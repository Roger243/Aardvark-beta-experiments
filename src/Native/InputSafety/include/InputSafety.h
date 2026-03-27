#pragma once

namespace WinAgent::Native::InputSafety {

class Module {
public:
    const char* Name() const noexcept { return "InputSafety"; }
    bool Initialize() noexcept;
};

}
