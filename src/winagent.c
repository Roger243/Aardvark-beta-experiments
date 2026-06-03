#include "winagent.h"

static NTSTATUS PriorityBoosterCompleteIrp(_In_ PIRP Irp, _In_ NTSTATUS Status, _In_ ULONG_PTR Information)
{
    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = Information;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

static BOOLEAN PriorityBoosterPriorityInRange(_In_ LONG Priority)
{
    return Priority >= PRIORITY_BOOSTER_MIN_PRIORITY && Priority <= PRIORITY_BOOSTER_MAX_PRIORITY;
}

static NTSTATUS PriorityBoosterReadRequest(
    _In_ PIRP Irp,
    _In_ PIO_STACK_LOCATION Stack,
    _Out_ PPRIORITY_BOOSTER_REQUEST Request)
{
    PVOID input = Stack->Parameters.DeviceIoControl.Type3InputBuffer;
    ULONG inputLength = Stack->Parameters.DeviceIoControl.InputBufferLength;

    if (input == NULL || inputLength < sizeof(*Request)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    __try {
        if (Irp->RequestorMode != KernelMode) {
            ProbeForRead(input, sizeof(*Request), __alignof(PRIORITY_BOOSTER_REQUEST));
        }

        RtlCopyMemory(Request, input, sizeof(*Request));
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return GetExceptionCode();
    }

    if (Request->ProcessId == 0 || !PriorityBoosterPriorityInRange(Request->Priority)) {
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

static NTSTATUS PriorityBoosterHandleSetPriority(
    _In_ PIRP Irp,
    _In_ PIO_STACK_LOCATION Stack,
    _Out_ PULONG_PTR Information)
{
    PRIORITY_BOOSTER_REQUEST request;
    NTSTATUS status;

    *Information = 0;
    status = PriorityBoosterReadRequest(Irp, Stack, &request);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    UNREFERENCED_PARAMETER(request);
    return STATUS_NOT_SUPPORTED;
}

static NTSTATUS PriorityBoosterHandleQueryVersion(
    _In_ PIRP Irp,
    _In_ PIO_STACK_LOCATION Stack,
    _Out_ PULONG_PTR Information)
{
    ULONG outputLength = Stack->Parameters.DeviceIoControl.OutputBufferLength;
    PPRIORITY_BOOSTER_VERSION_INFO version = (PPRIORITY_BOOSTER_VERSION_INFO)Irp->AssociatedIrp.SystemBuffer;

    *Information = 0;

    if (version == NULL || outputLength < sizeof(*version)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    RtlZeroMemory(version, sizeof(*version));
    version->Major = PRIORITY_BOOSTER_VERSION_MAJOR;
    version->Minor = PRIORITY_BOOSTER_VERSION_MINOR;
    version->DeviceType = PRIORITY_BOOSTER_DEVICE_TYPE;
    version->SetPriorityIoctl = PRIORITY_BOOSTER_IOCTL_SET_PRIORITY;
    *Information = sizeof(*version);

    return STATUS_SUCCESS;
}

NTSTATUS PriorityBoosterDispatchUnsupported(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    return PriorityBoosterCompleteIrp(Irp, STATUS_INVALID_DEVICE_REQUEST, 0);
}

NTSTATUS PriorityBoosterDispatchCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    return PriorityBoosterCompleteIrp(Irp, STATUS_SUCCESS, 0);
}

NTSTATUS PriorityBoosterDispatchDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp)
{
    PIO_STACK_LOCATION stack;
    ULONG_PTR information = 0;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(DeviceObject);

    stack = IoGetCurrentIrpStackLocation(Irp);

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_PRIORITY_BOOSTER_SET_PRIORITY:
        status = PriorityBoosterHandleSetPriority(Irp, stack, &information);
        break;
    case PRIORITY_BOOSTER_IOCTL_QUERY_VERSION:
        status = PriorityBoosterHandleQueryVersion(Irp, stack, &information);
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    return PriorityBoosterCompleteIrp(Irp, status, information);
}

void PriorityBoosterUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(PRIORITY_BOOSTER_SYMBOLIC_LINK_NAME);

    IoDeleteSymbolicLink(&symLink);

    if (DriverObject->DeviceObject != NULL) {
        IoDeleteDevice(DriverObject->DeviceObject);
    }
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNICODE_STRING devName = RTL_CONSTANT_STRING(PRIORITY_BOOSTER_DEVICE_NAME);
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(PRIORITY_BOOSTER_SYMBOLIC_LINK_NAME);
    PDEVICE_OBJECT deviceObject = NULL;
    NTSTATUS status;
    ULONG index;

    UNREFERENCED_PARAMETER(RegistryPath);

    status = IoCreateDevice(
        DriverObject,
        0,
        &devName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject);

    if (!NT_SUCCESS(status)) {
        KdPrint(("PriorityBooster: Failed to create device object (0x%08X)\n", status));
        return status;
    }

    status = IoCreateSymbolicLink(&symLink, &devName);

    if (!NT_SUCCESS(status)) {
        KdPrint(("PriorityBooster: Failed to create symbolic link (0x%08X)\n", status));
        IoDeleteDevice(deviceObject);
        return status;
    }

    for (index = 0; index <= IRP_MJ_MAXIMUM_FUNCTION; index++) {
        DriverObject->MajorFunction[index] = PriorityBoosterDispatchUnsupported;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = PriorityBoosterDispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = PriorityBoosterDispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PriorityBoosterDispatchDeviceControl;
    DriverObject->DriverUnload = PriorityBoosterUnload;

    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    KdPrint(("PriorityBooster: Driver loaded successfully\n"));
    return STATUS_SUCCESS;
}
