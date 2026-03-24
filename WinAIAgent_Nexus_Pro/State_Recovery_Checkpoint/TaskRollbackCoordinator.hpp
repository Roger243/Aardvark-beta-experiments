#ifndef WIN_AI_AGENT_NEXUS_PRO_STATE_RECOVERY_CHECKPOINT_TASKROLLBACKCOORDINATOR_HPP
#define WIN_AI_AGENT_NEXUS_PRO_STATE_RECOVERY_CHECKPOINT_TASKROLLBACKCOORDINATOR_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace win_ai_agent_nexus_pro::state_recovery_checkpoint {

struct SystemEvent final {
  std::string domain;
  nlohmann::json payload;
};

using TaskNode = std::variant<std::string, SystemEvent>;

class TaskRollbackCoordinator final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const TaskNode& node) const;
};

}  // namespace win_ai_agent_nexus_pro::state_recovery_checkpoint

#endif
