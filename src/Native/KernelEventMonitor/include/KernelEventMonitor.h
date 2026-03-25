#pragma once

namespace WinAgent::Native::KernelEventMonitor {

class Module {
public:
    const char* Name() const noexcept { return "KernelEventMonitor"; }
    bool Initialize() noexcept;
};

}
