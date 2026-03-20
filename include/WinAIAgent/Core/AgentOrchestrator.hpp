#ifndef WIN_AI_AGENT_CORE_AGENT_ORCHESTRATOR_HPP
#define WIN_AI_AGENT_CORE_AGENT_ORCHESTRATOR_HPP

#include <expected>
#include <string>

namespace win_ai_agent::core {

class AgentOrchestrator final {
 public:
  std::expected<void, std::string> InitializeSystem();
  void Run();
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_AGENT_ORCHESTRATOR_HPP
