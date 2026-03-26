#pragma once

namespace WinAgent::Native::HollowingProtector {
class Module {
public:
    const char* Name() const noexcept { return "HollowingProtector"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::HollowingProtector
