#ifndef WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_LOCAL_COMMAND_EXECUTOR_HPP
#define WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_LOCAL_COMMAND_EXECUTOR_HPP

#include "WinAIAgent/CoreEngine/ICommandExecutor.hpp"

namespace win_ai_agent::core {

class LocalCommandExecutor final : public ICommandExecutor {
 public:
  [[nodiscard]] CommandResult Execute(const CommandRequest& request) override;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_LOCAL_COMMAND_EXECUTOR_HPP
