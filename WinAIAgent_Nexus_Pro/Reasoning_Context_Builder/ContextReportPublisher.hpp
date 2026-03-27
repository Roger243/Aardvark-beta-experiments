#ifndef WIN_AI_AGENT_NEXUS_PRO_REASONING_CONTEXT_BUILDER_CONTEXT_REPORT_PUBLISHER_HPP
#define WIN_AI_AGENT_NEXUS_PRO_REASONING_CONTEXT_BUILDER_CONTEXT_REPORT_PUBLISHER_HPP

#include "AI_Orchestration_&_Communication/AgentOrchestrator.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace win_ai_agent_nexus_pro::reasoning_context_builder {

class ContextReportPublisher final {
 public:
  std::expected<void, std::string> Publish(ai_orchestration::AgentOrchestrator& bus,
                                           const nlohmann::json& report) const;
};

}  // namespace win_ai_agent_nexus_pro::reasoning_context_builder

#endif
