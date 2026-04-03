#include <ntddk.h>

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD SafeSkeletonUnload;

_Use_decl_annotations_
void SafeSkeletonUnload(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    KdPrint(("[SafeSkeleton] Driver unload invoked.\n"));
}

_Use_decl_annotations_
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = SafeSkeletonUnload;

    KdPrint(("[SafeSkeleton] Driver loaded successfully.\n"));
    KdPrint(("[SafeSkeleton] No devices, hooks, filters, or callbacks are registered.\n"));

    return STATUS_SUCCESS;
}
