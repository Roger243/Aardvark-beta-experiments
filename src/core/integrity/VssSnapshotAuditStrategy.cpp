#include "src/core/integrity/VssSnapshotAuditStrategy.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> VssSnapshotAuditStrategy::RunAudit() {
  const auto cleanup = manager_.CleanupOlderThan24h();
  if (!cleanup) {
    return std::unexpected(cleanup.error());
  }

  nlohmann::json report;
  report["module"] = "VSS_Snapshot_Provider";
  report["cleanup_deleted_count"] = cleanup->size();
  report["zero_persistence"] = true;
  return report;
}

}  // namespace win_ai_agent::nexus_pro
