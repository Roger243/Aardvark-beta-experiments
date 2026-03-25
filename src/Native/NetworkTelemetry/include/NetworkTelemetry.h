#pragma once

namespace WinAgent::Native::NetworkTelemetry {

class Module {
public:
    const char* Name() const noexcept { return "NetworkTelemetry"; }
    bool Initialize() noexcept;
};

}
