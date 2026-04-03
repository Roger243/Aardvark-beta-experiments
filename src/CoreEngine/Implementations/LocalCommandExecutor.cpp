#include "WinAIAgent/CoreEngine/Implementations/LocalCommandExecutor.hpp"

#include <chrono>
#include <sstream>
#include <string>

namespace win_ai_agent::core {
namespace {

std::string BuildShellCommand(const CommandRequest& request) {
  std::ostringstream cmd;
  cmd << request.executable;
  for (const auto& arg : request.arguments) {
    cmd << " \"" << arg << "\"";
  }
  return cmd.str();
}

}  // namespace

CommandResult LocalCommandExecutor::Execute(const CommandRequest& request) {
  const auto start = std::chrono::steady_clock::now();

  if (request.executable.empty()) {
    return CommandResult{.exit_code = -1, .error = "Executable cannot be empty"};
  }

  const std::string cmd = BuildShellCommand(request);
  const int exit_code = std::system(cmd.c_str());

  const auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);

  CommandResult result;
  result.exit_code = exit_code;
  result.duration = duration;
  if (exit_code != 0) {
    result.error = "Command failed: " + cmd;
  }
  return result;
}

}  // namespace win_ai_agent::core
