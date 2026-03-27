#pragma once

namespace WinAgent::Native::UsbFilter {
class Module {
public:
    const char* Name() const noexcept { return "UsbFilter"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::UsbFilter
