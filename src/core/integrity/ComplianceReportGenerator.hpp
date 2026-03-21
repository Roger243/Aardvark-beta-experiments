#pragma once

#include "src/core/integrity/ComplianceAggregator.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class ComplianceReportGenerator {
 public:
  explicit ComplianceReportGenerator(ComplianceAggregator aggregator);
  [[nodiscard]] std::expected<void, WinErrorStruct> WriteIntegrityManifest(const std::string& output_path) const;

 private:
  ComplianceAggregator aggregator_;
};

}  // namespace win_ai_agent::nexus_pro
