#pragma once

namespace WinAgent::Native::HookAudit {

class Module {
public:
    const char* Name() const noexcept { return "HookAudit"; }
    bool Initialize() noexcept;
};

}
