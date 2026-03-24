#pragma once

#include <string>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual Common::OperationResult Initialize(const std::string& sink) = 0;
    virtual void Log(Common::Severity severity, const std::string& component, const std::string& message) = 0;
    virtual void Flush() = 0;
};

}  // namespace EnterpriseSuite::Modules
