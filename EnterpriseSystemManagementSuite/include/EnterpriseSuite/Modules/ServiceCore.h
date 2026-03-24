#pragma once

#include <string>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

class IServiceCore {
public:
    virtual ~IServiceCore() = default;

    virtual Common::OperationResult Initialize() = 0;
    virtual Common::OperationResult Start() = 0;
    virtual Common::OperationResult Stop() = 0;
    virtual Common::OperationResult Pause() = 0;
    virtual Common::OperationResult Resume() = 0;
    virtual bool IsRunning() const = 0;
    virtual std::string ServiceName() const = 0;
};

}  // namespace EnterpriseSuite::Modules
