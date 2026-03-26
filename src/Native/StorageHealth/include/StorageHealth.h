#pragma once

namespace WinAgent::Native::StorageHealth {

class Module {
public:
    const char* Name() const noexcept { return "StorageHealth"; }
    bool Initialize() noexcept;
};

}
