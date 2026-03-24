#pragma once

#include "src/core/integrity/IntegrityAuditStrategy.hpp"

#include <memory>
#include <vector>

namespace win_ai_agent::nexus_pro {

class ComplianceAggregator {
 public:
  void AddStrategy(std::shared_ptr<IntegrityAuditStrategy> strategy);
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> Collect() const;

 private:
  std::vector<std::shared_ptr<IntegrityAuditStrategy>> strategies_;
};

}  // namespace win_ai_agent::nexus_pro
