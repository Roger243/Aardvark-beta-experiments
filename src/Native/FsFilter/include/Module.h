#pragma once

namespace WinAgent::Native::FsFilter {
class Module {
public:
    const char* Name() const noexcept { return "FsFilter"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::FsFilter
