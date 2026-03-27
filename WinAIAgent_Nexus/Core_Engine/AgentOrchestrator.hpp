#ifndef WIN_AI_AGENT_NEXUS_CORE_ENGINE_AGENT_ORCHESTRATOR_HPP
#define WIN_AI_AGENT_NEXUS_CORE_ENGINE_AGENT_ORCHESTRATOR_HPP

#include <expected>
#include <jthread>
#include <string>

namespace win_ai_agent_nexus::core_engine {

class InternalMessageBus;
class GuardDog_Safety;

/**
 * @brief Primary Nexus state machine coordinating all subsystem lifecycles.
 */
class AgentOrchestrator final {
 public:
  AgentOrchestrator(InternalMessageBus& bus, GuardDog_Safety& guard_dog);
  ~AgentOrchestrator();

  /**
   * @brief Perform staged startup validation and launch the orchestration loop.
   */
  std::expected<void, std::string> Start();

  /**
   * @brief Request shutdown and stop all running jobs.
   */
  void Stop();

 private:
  void RunStateMachine(std::stop_token stop_token);

  InternalMessageBus& bus_;
  GuardDog_Safety& guard_dog_;
  std::jthread orchestration_thread_;
};

}  // namespace win_ai_agent_nexus::core_engine

#endif
