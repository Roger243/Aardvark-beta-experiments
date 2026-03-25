#pragma once

namespace WinAgent::Native::DebugSignalAudit {

class Module {
public:
    const char* Name() const noexcept { return "DebugSignalAudit"; }
    bool Initialize() noexcept;
};

}
