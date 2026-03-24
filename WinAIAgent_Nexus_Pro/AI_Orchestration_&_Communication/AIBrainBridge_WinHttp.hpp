#ifndef WIN_AI_AGENT_NEXUS_PRO_AI_ORCHESTRATION_AIBRAINBRIDGE_WINHTTP_HPP
#define WIN_AI_AGENT_NEXUS_PRO_AI_ORCHESTRATION_AIBRAINBRIDGE_WINHTTP_HPP

#include "AI_Orchestration_&_Communication/AgentOrchestrator.hpp"

#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::ai_orchestration {

class AIBrainBridge_WinHttp final {
 public:
  std::expected<void, std::string> Initialize(AgentOrchestrator& bus);
  void OnExperienceEvent(const ExperienceEvent& event);
};

}  // namespace win_ai_agent_nexus_pro::ai_orchestration

#endif
