#include "WinAIAgent/Advanced/LogonOptimizationEngine.hpp"

#include <windows.h>
#include <mutex>

namespace win_ai_agent::advanced {

LogonOptimizationEngine::LogonOptimizationEngine(
    const win_ai_agent::kernel::WinKernelManager& kernel,
    const win_ai_agent::network::SecureCommunicator& communicator)
    : kernel_(kernel), communicator_(communicator) {}

std::expected<void, std::string> LogonOptimizationEngine::CollectAndSendBootBottlenecks() const {
  std::shared_lock lock(mutex_);

  const auto processes = kernel_.EnumerateProcesses();
  nlohmann::json payload;
  payload["event"] = "boot_optimization_snapshot";
  payload["process_count"] = processes.size();
  payload["note"] = "ETW integration stub: collect Microsoft-Windows-Diagnostics-Performance provider";

  return communicator_.SendTelemetryJson("https://example.invalid/perf", payload);
}

}  // namespace win_ai_agent::advanced
