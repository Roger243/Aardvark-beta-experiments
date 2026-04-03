#ifndef WIN_AI_AGENT_CORE_ENGINE_CORE_TYPES_HPP
#define WIN_AI_AGENT_CORE_ENGINE_CORE_TYPES_HPP

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace win_ai_agent::core {

enum class LogLevel : std::uint8_t {
  kTrace,
  kInfo,
  kWarn,
  kError,
};

struct CommandRequest final {
  std::string executable;
  std::vector<std::string> arguments;
  std::chrono::milliseconds timeout{30000};
};

struct CommandResult final {
  int exit_code{-1};
  std::string output;
  std::string error;
  std::chrono::milliseconds duration{0};
};

struct TaskSpec final {
  std::string id;
  std::string description;
  std::function<void()> work;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_CORE_TYPES_HPP
