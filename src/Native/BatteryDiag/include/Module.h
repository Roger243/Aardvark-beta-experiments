#pragma once

namespace WinAgent::Native::BatteryDiag {
class Module {
public:
    const char* Name() const noexcept { return "BatteryDiag"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::BatteryDiag
