#include "WinAIAgent/CoreEngine/CoreEngine.hpp"

#include <utility>

namespace win_ai_agent::core {
namespace {

void LogIfAvailable(const std::shared_ptr<ILogger>& logger,
                    const LogLevel level,
                    std::string_view component,
                    std::string message) {
  if (logger) {
    logger->Log(level, component, message);
  }
}

}  // namespace

CoreEngine::CoreEngine(std::unique_ptr<ICommandExecutor> command_executor,
                       std::shared_ptr<ILogger> logger,
                       std::unique_ptr<ITaskManager> task_manager)
    : command_executor_(std::move(command_executor)),
      logger_(std::move(logger)),
      task_manager_(std::move(task_manager)) {}

bool CoreEngine::RegisterModule(std::unique_ptr<IModule> module) {
  if (!module) {
    LogIfAvailable(logger_, LogLevel::kError, "CoreEngine", "Attempted to register a null module");
    return false;
  }

  const std::string module_name(module->Name());
  auto [iter, inserted] = modules_.emplace(module_name, std::move(module));
  (void)iter;
  if (!inserted) {
    LogIfAvailable(logger_, LogLevel::kWarn, "CoreEngine",
                   "Module registration skipped due to duplicate name: " + module_name);
  }
  return inserted;
}

bool CoreEngine::StartModule(std::string_view module_name) {
  auto it = modules_.find(std::string(module_name));
  if (it == modules_.end()) {
    LogIfAvailable(logger_, LogLevel::kError, "CoreEngine", "Module not found: " + std::string(module_name));
    return false;
  }

  const bool started = it->second->Start();
  LogIfAvailable(logger_, started ? LogLevel::kInfo : LogLevel::kError, "CoreEngine",
                 (started ? "Module started: " : "Module failed to start: ") + std::string(module_name));

  return started;
}

void CoreEngine::StopAllModules() noexcept {
  for (auto& [name, module] : modules_) {
    module->Stop();
    LogIfAvailable(logger_, LogLevel::kInfo, "CoreEngine", "Module stopped: " + name);
  }
}

CommandResult CoreEngine::ExecuteCommand(const CommandRequest& request) {
  if (!command_executor_) {
    LogIfAvailable(logger_, LogLevel::kError, "CoreEngine", "No command executor configured");
    return CommandResult{.exit_code = -1, .error = "No command executor configured"};
  }
  return command_executor_->Execute(request);
}

bool CoreEngine::SubmitTask(TaskSpec task) {
  if (!task_manager_) {
    LogIfAvailable(logger_, LogLevel::kError, "CoreEngine", "No task manager configured");
    return false;
  }

  LogIfAvailable(logger_, LogLevel::kTrace, "CoreEngine",
                 "Submitting task: " + task.id + " - " + task.description);
  return task_manager_->Submit(std::move(task));
}

}  // namespace win_ai_agent::core
