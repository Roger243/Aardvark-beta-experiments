#include "Reasoning_Context_Builder/ContextReportPublisher.hpp"

namespace win_ai_agent_nexus_pro::reasoning_context_builder {

std::expected<void, std::string> ContextReportPublisher::Publish(
    ai_orchestration::AgentOrchestrator& bus, const nlohmann::json& report) const {
  ai_orchestration::ExperienceEvent event{
      .action_id = "context_report",
      .state_snapshot_json = report.dump(),
      .outcome = "emitted",
      .feedback_weight = 1.0,
  };
  return bus.Publish(event);
}

}  // namespace win_ai_agent_nexus_pro::reasoning_context_builder
