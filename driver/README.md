# Driver module (safe foundation)

This folder contains a **minimal kernel-mode driver skeleton** intended as a safe starting point for future driver development.

## Included module
- `SafeSkeleton/`
  - `SafeSkeleton.c`: minimal `DriverEntry` and unload routine.
  - `SafeSkeleton.vcxproj`: isolated WDK project (x64 + ARM64, Debug/Release).
  - `SafeSkeleton.vcxproj.filters`: Visual Studio filter metadata.

## Safety characteristics
The skeleton intentionally does **not** perform risky kernel operations:
- no device object creation
- no IRP dispatch table customization
- no callback registration
- no process/thread manipulation
- no memory patching or hooking

It only logs load/unload messages with `KdPrint` and returns `STATUS_SUCCESS`.

## Build guidance (Windows host)
1. Install Visual Studio 2019/2022 with WDK.
2. Open `driver/SafeSkeleton/SafeSkeleton.vcxproj`.
3. Select configuration/platform (`Debug|x64`, `Release|x64`, `Debug|ARM64`, `Release|ARM64`).
4. Build from Visual Studio.

## Isolation from main logic
This module is intentionally not linked into the main CMake targets (`agent_modules`, `ai_agent`, `ai_agent_service`).
It lives as a separate project to preserve safety and maintainability boundaries.
