#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "EnterpriseSuite/Common/Types.h"

namespace EnterpriseSuite::Modules {

struct HardwareHealthSample {
    std::string component;
    std::string metric;
    std::int64_t value{0};
    std::string units;
};

class IHardwareDiagnostics {
public:
    virtual ~IHardwareDiagnostics() = default;

    virtual Common::OperationResult RunQuickCheck() = 0;
    virtual Common::OperationResult RunDeepCheck() = 0;
    virtual std::vector<HardwareHealthSample> CollectHealthSamples() const = 0;
    virtual std::string BuildSummaryReport() const = 0;
};

}  // namespace EnterpriseSuite::Modules
