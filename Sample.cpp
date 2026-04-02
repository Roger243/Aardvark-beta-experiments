#include <ntddk.h>

#define DRIVER_TAG 'dcba'

UNICODE_STRING g_RegistryPath;

void SampleUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    if (g_RegistryPath.Buffer != nullptr) {
        ExFreePool(g_RegistryPath.Buffer);
        g_RegistryPath.Buffer = nullptr;
        g_RegistryPath.Length = 0;
        g_RegistryPath.MaximumLength = 0;
    }

    KdPrint(("Sample driver unload called\n"));
}

extern "C" NTSTATUS
DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    const USHORT allocSize = RegistryPath->Length + sizeof(WCHAR);

    g_RegistryPath.Buffer = static_cast<PWCH>(
        ExAllocatePool2(POOL_FLAG_PAGED, allocSize, DRIVER_TAG));

    if (g_RegistryPath.Buffer == nullptr) {
        KdPrint(("Failed to allocate memory for registry path\n"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    g_RegistryPath.Length = 0;
    g_RegistryPath.MaximumLength = allocSize;
    RtlCopyUnicodeString(&g_RegistryPath, RegistryPath);
    g_RegistryPath.Buffer[g_RegistryPath.Length / sizeof(WCHAR)] = L'\0';

    KdPrint(("Original registry path: %wZ\n", RegistryPath));
    KdPrint(("Copied registry path: %wZ\n", &g_RegistryPath));

    DriverObject->DriverUnload = SampleUnload;

    RTL_OSVERSIONINFOW info = { sizeof(info) };
    NTSTATUS versionStatus = RtlGetVersion(&info);
    if (NT_SUCCESS(versionStatus)) {
        KdPrint(("Windows Version: %lu.%lu.%lu\n",
            info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber));
    } else {
        KdPrint(("RtlGetVersion failed with status 0x%08X\n", versionStatus));
    }

    KdPrint(("Sample driver initialized successfully\n"));

    return STATUS_SUCCESS;
}
