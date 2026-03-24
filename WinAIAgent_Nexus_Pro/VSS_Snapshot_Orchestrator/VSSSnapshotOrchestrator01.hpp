#ifndef WIN_AI_AGENT_NEXUS_PRO_VSS_SNAPSHOT_ORCHESTRATOR_VSSSNAPSHOTORCHESTRATOR01_HPP
#define WIN_AI_AGENT_NEXUS_PRO_VSS_SNAPSHOT_ORCHESTRATOR_VSSSNAPSHOTORCHESTRATOR01_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <vss.h>
#include <vsbackup.h>
#include <string>


namespace win_ai_agent_nexus_pro::vss_snapshot_orchestrator {

class VSSSnapshotOrchestrator01 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::vss_snapshot_orchestrator

#endif
