#include "Task_Tree_Orchestrator/IntentPlanner.hpp"

namespace win_ai_agent_nexus_pro::task_tree_orchestrator {

std::expected<nlohmann::json, std::string> IntentPlanner::Execute(const TaskNode& node) const {
  nlohmann::json out;
  out["status"] = "ok";
  out["module"] = "IntentPlanner";
  out["atomic_events"] = std::holds_alternative<SystemEvent>(node) ? 1 : 0;
  return out;
}

std::expected<void, std::string> IntentPlanner::PublishExperience(
    ai_orchestration::AgentOrchestrator& bus, const nlohmann::json& result) const {
  ai_orchestration::ExperienceEvent event{
      .action_id = "task_tree_plan",
      .state_snapshot_json = result.dump(),
      .outcome = "planned",
      .feedback_weight = 1.0,
  };
  return bus.Publish(event);
}

}  // namespace win_ai_agent_nexus_pro::task_tree_orchestrator
