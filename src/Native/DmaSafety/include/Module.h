#pragma once

namespace WinAgent::Native::DmaSafety {
class Module {
public:
    const char* Name() const noexcept { return "DmaSafety"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::DmaSafety
