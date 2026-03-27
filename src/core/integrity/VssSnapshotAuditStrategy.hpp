#pragma once

#include "src/core/integrity/IntegrityAuditStrategy.hpp"
#include "src/core/integrity/VssSnapshotManager.hpp"

namespace win_ai_agent::nexus_pro {

class VssSnapshotAuditStrategy final : public IntegrityAuditStrategy {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> RunAudit() override;

 private:
  VssSnapshotManager manager_;
};

}  // namespace win_ai_agent::nexus_pro
