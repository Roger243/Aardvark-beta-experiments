#include "AI_Orchestration_&_Communication/AgentOrchestrator.hpp"

#include <chrono>
#include <thread>

namespace win_ai_agent_nexus_pro::ai_orchestration {
namespace {

std::string ResolveChannel(const BusMessage& message) {
  return std::visit(
      [](const auto& value) -> std::string {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, HealthTick>) {
          return "health";
        } else if constexpr (std::is_same_v<T, SafetyAlert>) {
          return "safety";
        } else if constexpr (std::is_same_v<T, ExperienceEvent>) {
          return "experience";
        }
        return "action";
      },
      message);
}

}  // namespace

AgentOrchestrator::AgentOrchestrator() = default;

AgentOrchestrator::~AgentOrchestrator() {
  Stop();
}

std::expected<void, std::string> AgentOrchestrator::Start() {
  std::scoped_lock lock(mutex_);
  if (started_) {
    return std::unexpected("AgentOrchestrator is already started");
  }

  dispatcher_.emplace([this](std::stop_token stop_token) {
    DispatchLoop(stop_token);
  });
  started_ = true;
  return {};
}

void AgentOrchestrator::Stop() {
  std::scoped_lock lock(mutex_);
  if (!started_) {
    return;
  }

  if (dispatcher_.has_value()) {
    dispatcher_->request_stop();
    dispatcher_->join();
    dispatcher_.reset();
  }
  started_ = false;
}

std::expected<void, std::string> AgentOrchestrator::Subscribe(
    const std::string& channel, Subscriber subscriber) {
  if (channel.empty()) {
    return std::unexpected("Subscribe channel cannot be empty");
  }
  if (!subscriber) {
    return std::unexpected("Subscriber callback is not valid");
  }

  std::scoped_lock lock(mutex_);
  subscribers_.emplace(channel, std::move(subscriber));
  return {};
}

std::expected<void, std::string> AgentOrchestrator::Publish(BusMessage message) {
  std::scoped_lock lock(mutex_);
  if (!started_) {
    return std::unexpected("AgentOrchestrator is not started");
  }

  queue_.push(std::move(message));
  return {};
}

void AgentOrchestrator::DispatchLoop(std::stop_token stop_token) {
  using namespace std::chrono_literals;

  while (!stop_token.stop_requested()) {
    BusMessage msg;
    bool has_msg = false;

    {
      std::scoped_lock lock(mutex_);
      if (!queue_.empty()) {
        msg = std::move(queue_.front());
        queue_.pop();
        has_msg = true;
      }
    }

    if (!has_msg) {
      std::this_thread::sleep_for(50ms);
      continue;
    }

    const std::string channel = ResolveChannel(msg);
    std::vector<Subscriber> callbacks;

    {
      std::scoped_lock lock(mutex_);
      auto [it, end] = subscribers_.equal_range(channel);
      for (; it != end; ++it) {
        callbacks.push_back(it->second);
      }
    }

    for (const auto& callback : callbacks) {
      callback(msg);
    }
  }
}

}  // namespace win_ai_agent_nexus_pro::ai_orchestration
