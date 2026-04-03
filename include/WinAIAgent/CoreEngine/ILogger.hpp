#ifndef WIN_AI_AGENT_CORE_ENGINE_ILOGGER_HPP
#define WIN_AI_AGENT_CORE_ENGINE_ILOGGER_HPP

#include "WinAIAgent/CoreEngine/CoreTypes.hpp"

#include <string>
#include <string_view>

namespace win_ai_agent::core {

class ILogger {
 public:
  virtual ~ILogger() = default;
  virtual void Log(LogLevel level, std::string_view component, std::string_view message) = 0;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_ILOGGER_HPP
