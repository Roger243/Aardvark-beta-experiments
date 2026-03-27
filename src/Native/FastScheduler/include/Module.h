#pragma once

namespace WinAgent::Native::FastScheduler {
class Module {
public:
    const char* Name() const noexcept { return "FastScheduler"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::FastScheduler
