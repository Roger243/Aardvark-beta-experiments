#pragma once

namespace WinAgent::Native::FirmwareAudit {

class Module {
public:
    const char* Name() const noexcept { return "FirmwareAudit"; }
    bool Initialize() noexcept;
};

}
