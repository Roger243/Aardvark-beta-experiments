#include "WinAIAgent/CoreEngine/CoreEngine.hpp"

#include <utility>

namespace win_ai_agent::core {

CoreEngine::CoreEngine(std::unique_ptr<ICommandExecutor> command_executor,
                       std::shared_ptr<ILogger> logger,
                       std::unique_ptr<ITaskManager> task_manager)
    : command_executor_(std::move(command_executor)),
      logger_(std::move(logger)),
      task_manager_(std::move(task_manager)) {}

bool CoreEngine::RegisterModule(std::unique_ptr<IModule> module) {
  if (!module) {
    return false;
  }

  const std::string module_name(module->Name());
  auto [_, inserted] = modules_.emplace(module_name, std::move(module));
  if (!inserted && logger_) {
    logger_->Log(LogLevel::kWarn, "CoreEngine", "Module registration skipped due to duplicate name: " + module_name);
  }
  return inserted;
}

bool CoreEngine::StartModule(std::string_view module_name) {
  auto it = modules_.find(std::string(module_name));
  if (it == modules_.end()) {
    if (logger_) {
      logger_->Log(LogLevel::kError, "CoreEngine", "Module not found: " + std::string(module_name));
    }
    return false;
  }

  const bool started = it->second->Start();
  if (logger_) {
    logger_->Log(started ? LogLevel::kInfo : LogLevel::kError,
                 "CoreEngine",
                 (started ? "Module started: " : "Module failed to start: ") + std::string(module_name));
  }

  return started;
}

void CoreEngine::StopAllModules() noexcept {
  for (auto& [name, module] : modules_) {
    module->Stop();
    if (logger_) {
      logger_->Log(LogLevel::kInfo, "CoreEngine", "Module stopped: " + name);
    }
  }
}

CommandResult CoreEngine::ExecuteCommand(const CommandRequest& request) {
  if (!command_executor_) {
    return CommandResult{.exit_code = -1, .error = "No command executor configured"};
  }
  return command_executor_->Execute(request);
}

bool CoreEngine::SubmitTask(TaskSpec task) {
  if (!task_manager_) {
    return false;
  }

  if (logger_) {
    logger_->Log(LogLevel::kTrace, "CoreEngine", "Submitting task: " + task.id + " - " + task.description);
  }
  return task_manager_->Submit(std::move(task));
}

}  // namespace win_ai_agent::core
