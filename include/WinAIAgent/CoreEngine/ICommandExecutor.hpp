#ifndef WIN_AI_AGENT_CORE_ENGINE_I_COMMAND_EXECUTOR_HPP
#define WIN_AI_AGENT_CORE_ENGINE_I_COMMAND_EXECUTOR_HPP

#include "WinAIAgent/CoreEngine/CoreTypes.hpp"

namespace win_ai_agent::core {

class ICommandExecutor {
 public:
  virtual ~ICommandExecutor() = default;
  [[nodiscard]] virtual CommandResult Execute(const CommandRequest& request) = 0;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_I_COMMAND_EXECUTOR_HPP
