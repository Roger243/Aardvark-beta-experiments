#pragma once

namespace WinAgent::Native::GuardedCapture {

class Module {
public:
    const char* Name() const noexcept { return "GuardedCapture"; }
    bool Initialize() noexcept;
};

}
