#ifndef WIN_AI_AGENT_CORE_ENGINE_CORE_ENGINE_HPP
#define WIN_AI_AGENT_CORE_ENGINE_CORE_ENGINE_HPP

#include "WinAIAgent/CoreEngine/ICommandExecutor.hpp"
#include "WinAIAgent/CoreEngine/ILogger.hpp"
#include "WinAIAgent/CoreEngine/IModule.hpp"
#include "WinAIAgent/CoreEngine/ITaskManager.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace win_ai_agent::core {

class CoreEngine final {
 public:
  CoreEngine(std::unique_ptr<ICommandExecutor> command_executor,
             std::shared_ptr<ILogger> logger,
             std::unique_ptr<ITaskManager> task_manager);

  bool RegisterModule(std::unique_ptr<IModule> module);
  bool StartModule(std::string_view module_name);
  void StopAllModules() noexcept;

  [[nodiscard]] CommandResult ExecuteCommand(const CommandRequest& request);
  bool SubmitTask(TaskSpec task);

 private:
  std::unique_ptr<ICommandExecutor> command_executor_;
  std::shared_ptr<ILogger> logger_;
  std::unique_ptr<ITaskManager> task_manager_;
  std::unordered_map<std::string, std::unique_ptr<IModule>> modules_;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_CORE_ENGINE_HPP
