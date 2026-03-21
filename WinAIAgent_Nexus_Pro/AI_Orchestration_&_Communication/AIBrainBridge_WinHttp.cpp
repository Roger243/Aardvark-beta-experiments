#include "AI_Orchestration_&_Communication/AIBrainBridge_WinHttp.hpp"

#include <iostream>

namespace win_ai_agent_nexus_pro::ai_orchestration {

std::expected<void, std::string> AIBrainBridge_WinHttp::Initialize(AgentOrchestrator& bus) {
  return bus.Subscribe("experience", [this](const BusMessage& message) {
    if (std::holds_alternative<ExperienceEvent>(message)) {
      OnExperienceEvent(std::get<ExperienceEvent>(message));
    }
  });
}

void AIBrainBridge_WinHttp::OnExperienceEvent(const ExperienceEvent& event) {
  std::cout << "[AIBrainBridge] action_id=" << event.action_id << " outcome=" << event.outcome
            << '\n';
}

}  // namespace win_ai_agent_nexus_pro::ai_orchestration
