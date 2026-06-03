# PriorityBooster Kernel Driver

PriorityBooster is a Windows 11 WDM kernel-mode driver scaffold. It creates a kernel device named `\Device\PriorityBooster`, publishes `\??\PriorityBooster`, and exposes a small IOCTL contract for system-level integrations.

This repository does not bypass Windows kernel security. Build, signing, installation, and loading must happen through the Windows Driver Kit, test-signing, or production signing policy on Windows 11.

## Kernel contract

- Device type: `0x8000`
- Device name: `\Device\PriorityBooster`
- Symbolic link: `\??\PriorityBooster`
- Priority request IOCTL: `IOCTL_PRIORITY_BOOSTER_SET_PRIORITY` / `PRIORITY_BOOSTER_IOCTL_SET_PRIORITY`
- Query version IOCTL: `PRIORITY_BOOSTER_IOCTL_QUERY_VERSION`

`IOCTL_PRIORITY_BOOSTER_SET_PRIORITY` keeps the requested `METHOD_NEITHER` contract and validates caller buffers in kernel mode. The current handler validates the request and returns `STATUS_NOT_SUPPORTED` instead of changing arbitrary process scheduling from the kernel.

## Files

- `include/winagent.h`: public kernel contract and IOCTL definitions.
- `src/winagent.c`: WDM `DriverEntry`, device creation, dispatch routines, IOCTL handlers, and unload path.
- `winagent.inf`: Windows 11 x64 root-enumerated kernel-driver install metadata.
- `winagent.vcxproj`: Visual Studio / WDK kernel-mode driver project.
- `docs/wdk/priority_booster_contract.h`: standalone WDK contract header.

## Build on Windows 11

Install Visual Studio with the Windows Driver Kit, then build from a Developer Command Prompt:

```cmd
msbuild winagent.vcxproj /p:Configuration=Release /p:Platform=x64
```

## Install for test only

Use a Windows 11 test machine configured for test-signed drivers:

```cmd
pnputil /add-driver winagent.inf /install
sc.exe query PriorityBooster
```

## Notes

A real `TYPE : 1 KERNEL_DRIVER` service is produced by the signed `.sys` driver and INF, not by granting extra privileges to a user-mode executable. The driver creates and removes the `PriorityBooster` device and symbolic link with `IoCreateDevice`, `IoCreateSymbolicLink`, `IoDeleteSymbolicLink`, and `IoDeleteDevice`, while intentionally avoiding stealth, persistence tricks, token manipulation, and arbitrary process-kill logic.
