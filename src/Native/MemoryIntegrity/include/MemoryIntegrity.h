#pragma once

namespace WinAgent::Native::MemoryIntegrity {

class Module {
public:
    const char* Name() const noexcept { return "MemoryIntegrity"; }
    bool Initialize() noexcept;
};

}
