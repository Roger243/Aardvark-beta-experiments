#pragma once

namespace WinAgent::Native::MemoryLeakDetector {
class Module {
public:
    const char* Name() const noexcept { return "MemoryLeakDetector"; }
    bool InitializeDiagnosticMode() noexcept;
};
} // namespace WinAgent::Native::MemoryLeakDetector
