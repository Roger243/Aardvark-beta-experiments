#pragma once

namespace WinAgent::Native::RuntimeIsolator {

class Module {
public:
    const char* Name() const noexcept { return "RuntimeIsolator"; }
    bool Initialize() noexcept;
};

}
