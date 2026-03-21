#ifndef WIN_AI_AGENT_SECURITY_SCRIPT_EXECUTOR_HPP
#define WIN_AI_AGENT_SECURITY_SCRIPT_EXECUTOR_HPP

#include <chrono>
#include <cstddef>
#include <string>

namespace win_ai_agent::security {

enum class ScriptType {
  PowerShell,
  Cmd,
};

struct ScriptExecutionResult final {
  bool success{};
  DWORD exit_code{};
  bool timed_out{};
  std::string circular_log;
  std::string error;
};

class ScriptExecutor final {
 public:
  ScriptExecutor();
  ~ScriptExecutor();

  ScriptExecutionResult ExecuteReceivedScript(const std::string& script,
                                              ScriptType type,
                                              std::chrono::milliseconds timeout,
                                              std::size_t circular_buffer_bytes = 64 * 1024) const;

 private:
  bool VerifyScriptSignature(const std::wstring& path) const;
};

}  // namespace win_ai_agent::security

#endif  // WIN_AI_AGENT_SECURITY_SCRIPT_EXECUTOR_HPP
