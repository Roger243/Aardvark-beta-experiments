#pragma once

namespace WinAgent::Native::PipeBridge {
class Module {
public:
    const char* Name() const noexcept { return "PipeBridge"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::PipeBridge
