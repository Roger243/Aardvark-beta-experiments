# Enterprise System Management Suite (Interfaces)

This directory contains a clean-architecture interface layer for an enterprise Windows system management suite.

## Modules

- `ServiceCore`
- `PrivilegeChecker`
- `SystemConfigMonitor`
- `LocalCommServer`
- `CommandParser`
- `TelemetryProvider`
- `ConfigManager`
- `Logger`
- `HardwareDiagnostics`
- `TaskScheduler`

Each module is defined as an abstract interface in `include/EnterpriseSuite/Modules/*.h` so concrete implementations can be developed and tested independently.
