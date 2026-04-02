#ifndef WIN_AI_AGENT_CORE_ENGINE_I_TASK_MANAGER_HPP
#define WIN_AI_AGENT_CORE_ENGINE_I_TASK_MANAGER_HPP

#include "WinAIAgent/CoreEngine/CoreTypes.hpp"

namespace win_ai_agent::core {

class ITaskManager {
 public:
  virtual ~ITaskManager() = default;
  virtual bool Submit(TaskSpec task) = 0;
  virtual void Shutdown() = 0;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_I_TASK_MANAGER_HPP
