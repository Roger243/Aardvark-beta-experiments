# Command Orchestration Module

This module provides a lightweight orchestration slice for command dispatch and task scheduling.

## Structure
- `controllers/`: entry points for API/CLI wiring.
- `services/`: business logic for dispatching and scheduling.
- `models/`: immutable data contracts.
- `utils/`: shared helpers (validation, retry policies).
- `interfaces/`: protocol abstractions.
- `tests/`: unit-level behavior tests.

The module is intentionally framework-agnostic and can be wrapped from CLI/API layers.
