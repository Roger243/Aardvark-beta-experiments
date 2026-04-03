# WinAgent Core Engine (Kernel-Inspired Architecture)

## Why this architecture
WinAgent already contains many system-facing components (network, security, kernel diagnostics, orchestration helpers), but lacks a single, explicit core runtime contract. This architecture introduces a kernel-inspired split:

- **Execution** (process/command dispatch)
- **Modules** (load/start/stop lifecycle)
- **Logging** (centralized observability stream)
- **Task manager** (cooperative scheduling queue)

## Existing reusable components identified
From current repository structure, these subsystems can plug into the core engine as modules:

- `src/Kernel/*` and `include/WinAIAgent/Kernel/*` → kernel/system telemetry module adapters.
- `src/Network/*` and `include/WinAIAgent/Network/*` → secure connectivity and transport modules.
- `src/Security/*` and `include/WinAIAgent/Security/*` → policy and audit modules.
- `src/AILogic/*` and `include/WinAIAgent/AILogic/*` → decisioning and planning modules.
- `src/utils/*` → shared low-level helpers (error and handle wrappers).

## New core interfaces
Added under `include/WinAIAgent/CoreEngine`:

- `ICommandExecutor.hpp`: OS/process execution abstraction.
- `IModule.hpp`: strict module lifecycle contract (`Start/Stop`).
- `ILogger.hpp`: centralized structured logging contract.
- `ITaskManager.hpp`: queue/scheduling boundary.
- `CoreEngine.hpp`: orchestrator that wires all contracts.
- `CoreTypes.hpp`: shared DTOs for command/task/log primitives.

## Runtime behavior
`CoreEngine` now provides:

1. **Module registry** with duplicate protection.
2. **Lifecycle controls** (`RegisterModule`, `StartModule`, `StopAllModules`).
3. **Command execution dispatch** through `ICommandExecutor`.
4. **Task submission path** through `ITaskManager`.
5. **Cross-cutting logs** through `ILogger`.

## Integration approach
- No existing modules were overwritten.
- New core contracts are additive and can be adopted incrementally.
- Legacy modules can be wrapped behind `IModule` adapters to migrate safely.

## Next recommended step
Implement concrete adapters:
- `KernelModuleAdapter` (wrap `win_ai_agent::kernel::WinKernelManager`)
- `SecurityModuleAdapter` (wrap registry/script/vault services)
- `NetworkModuleAdapter` (wrap `NetworkManager`/`SecureCommunicator`)


## Driver foundation module
A safe and isolated kernel-mode foundation now exists under `driver/SafeSkeleton`:
- Minimal `DriverEntry` + unload only.
- No hooks/callbacks/device objects/IRP dispatch modifications.
- Kept outside main CMake targets to prevent accidental coupling with user-mode runtime.
