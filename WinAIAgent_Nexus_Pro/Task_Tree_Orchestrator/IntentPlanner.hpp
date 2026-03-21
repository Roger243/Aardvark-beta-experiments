#ifndef WIN_AI_AGENT_NEXUS_PRO_TASK_TREE_ORCHESTRATOR_INTENTPLANNER_HPP
#define WIN_AI_AGENT_NEXUS_PRO_TASK_TREE_ORCHESTRATOR_INTENTPLANNER_HPP

#include "AI_Orchestration_&_Communication/AgentOrchestrator.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace win_ai_agent_nexus_pro::task_tree_orchestrator {

struct SystemEvent final {
  std::string domain;
  nlohmann::json payload;
};

using TaskNode = std::variant<std::string, SystemEvent>;

class IntentPlanner final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const TaskNode& node) const;
  std::expected<void, std::string> PublishExperience(
      ai_orchestration::AgentOrchestrator& bus, const nlohmann::json& result) const;
};

}  // namespace win_ai_agent_nexus_pro::task_tree_orchestrator

#endif
