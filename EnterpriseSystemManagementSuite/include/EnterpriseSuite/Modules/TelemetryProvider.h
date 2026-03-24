#pragma once

#include <string>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct TelemetryRecord {
    std::string category;
    Common::Severity severity{Common::Severity::kInfo};
    std::string message;
};

class ITelemetryProvider {
public:
    virtual ~ITelemetryProvider() = default;

    virtual Common::OperationResult Start() = 0;
    virtual Common::OperationResult Stop() = 0;
    virtual Common::OperationResult Publish(const TelemetryRecord& record) = 0;
    virtual std::vector<TelemetryRecord> Snapshot() const = 0;
};

}  // namespace EnterpriseSuite::Modules
