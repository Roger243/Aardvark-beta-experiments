#ifndef WIN_AI_AGENT_NEXUS_PRO_AI_ORCHESTRATION_AGENT_ORCHESTRATOR_HPP
#define WIN_AI_AGENT_NEXUS_PRO_AI_ORCHESTRATION_AGENT_ORCHESTRATOR_HPP

#include <any>
#include <expected>
#include <functional>
#include <jthread>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <unordered_map>
#include <variant>

namespace win_ai_agent_nexus_pro::ai_orchestration {

/**
 * @brief Message types exchanged between modules through the orchestration bus.
 */
struct HealthTick final {
  std::string module;
  double cpu_percent{};
  double memory_percent{};
};

struct SafetyAlert final {
  std::string module;
  std::string reason;
};

struct ActionRequest final {
  std::string capability;
  std::string payload_json;
};

struct ExperienceEvent final {
  std::string action_id;
  std::string state_snapshot_json;
  std::string outcome;
  std::any feedback_weight;
};

using BusMessage = std::variant<HealthTick, SafetyAlert, ActionRequest, ExperienceEvent>;
using Subscriber = std::function<void(const BusMessage&)>;

/**
 * @brief Central asynchronous orchestrator for WinAIAgent_Nexus_Pro.
 *
 * This class provides:
 * - an in-process message bus,
 * - thread-managed dispatch loop,
 * - structured error handling with std::expected,
 * - cooperative shutdown semantics via std::jthread.
 */
class AgentOrchestrator final {
 public:
  AgentOrchestrator();
  ~AgentOrchestrator();

  AgentOrchestrator(const AgentOrchestrator&) = delete;
  AgentOrchestrator& operator=(const AgentOrchestrator&) = delete;

  /**
   * @brief Starts the orchestrator dispatch loop.
   */
  std::expected<void, std::string> Start();

  /**
   * @brief Stops all orchestrator worker activity.
   */
  void Stop();

  /**
   * @brief Registers a subscriber callback for a logical module channel.
   */
  std::expected<void, std::string> Subscribe(const std::string& channel,
                                             Subscriber subscriber);

  /**
   * @brief Enqueues a message for asynchronous fan-out delivery.
   */
  std::expected<void, std::string> Publish(BusMessage message);

 private:
  void DispatchLoop(std::stop_token stop_token);

  std::mutex mutex_;
  std::queue<BusMessage> queue_;
  std::unordered_multimap<std::string, Subscriber> subscribers_;
  std::optional<std::jthread> dispatcher_;
  bool started_{false};
};

}  // namespace win_ai_agent_nexus_pro::ai_orchestration

#endif
