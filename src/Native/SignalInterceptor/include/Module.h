#pragma once

namespace WinAgent::Native::SignalInterceptor {
class Module {
public:
    const char* Name() const noexcept { return "SignalInterceptor"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::SignalInterceptor
