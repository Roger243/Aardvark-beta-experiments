#include "Core_Engine/AgentOrchestrator.hpp"

#include "Core_Engine/GuardDog_Safety.hpp"
#include "Core_Engine/InternalMessageBus.hpp"

#include <chrono>
#include <nlohmann/json.hpp>
#include <thread>

namespace win_ai_agent_nexus::core_engine {

AgentOrchestrator::AgentOrchestrator(InternalMessageBus& bus, GuardDog_Safety& guard_dog)
    : bus_(bus), guard_dog_(guard_dog) {}

AgentOrchestrator::~AgentOrchestrator() {
  Stop();
}

std::expected<void, std::string> AgentOrchestrator::Start() {
  if (auto safe = guard_dog_.Initialize(); !safe) {
    return std::unexpected("GuardDog initialization failed: " + safe.error());
  }

  orchestration_thread_ = std::jthread([this](std::stop_token st) {
    RunStateMachine(st);
  });

  return {};
}

void AgentOrchestrator::Stop() {
  if (orchestration_thread_.joinable()) {
    orchestration_thread_.request_stop();
    orchestration_thread_.join();
  }
}

void AgentOrchestrator::RunStateMachine(std::stop_token stop_token) {
  using namespace std::chrono_literals;

  while (!stop_token.stop_requested()) {
    nlohmann::json heartbeat{
        {"module", "AgentOrchestrator"},
        {"status", "running"},
    };

    (void)heartbeat;
    (void)bus_.Initialize();
    std::this_thread::sleep_for(500ms);
  }
}

}  // namespace win_ai_agent_nexus::core_engine
