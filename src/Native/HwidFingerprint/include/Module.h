#pragma once

namespace WinAgent::Native::HwidFingerprint {
class Module {
public:
    const char* Name() const noexcept { return "HwidFingerprint"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::HwidFingerprint
