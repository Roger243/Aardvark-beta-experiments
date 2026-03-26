#ifndef WIN_AI_AGENT_KERNEL_SYSTEM_RELIABILITY_DIAGNOSTICS_HPP
#define WIN_AI_AGENT_KERNEL_SYSTEM_RELIABILITY_DIAGNOSTICS_HPP

#include <windows.h>

#include <expected>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>

namespace win_ai_agent::kernel {

/**
 * @brief High-level process metadata collected from Toolhelp snapshots.
 */
struct ProcessMetadata final {
  std::wstring image_name;
  DWORD pid{};
  DWORD thread_count{};
  LONG base_priority{};
};

/**
 * @brief Per-process performance metrics from PSAPI and process timing APIs.
 */
struct ProcessPerformance final {
  DWORD pid{};
  SIZE_T working_set_size{};
  SIZE_T peak_working_set_size{};
  DWORD page_fault_count{};
  double cpu_percent{};
};

/**
 * @brief Thread context snapshot for zombie/hang diagnostics.
 */
struct ThreadDiagnostic final {
  DWORD pid{};
  DWORD tid{};
  bool context_available{};
  std::uintptr_t instruction_pointer{};
};

/**
 * @brief Reliability-focused kernel bridge used for process intelligence and controlled remediation.
 */
class SystemReliabilityDiagnostics final {
 public:
  /**
   * @brief Initialize event logging handles and internal state.
   */
  SystemReliabilityDiagnostics();

  /**
   * @brief Release OS resources.
   */
  ~SystemReliabilityDiagnostics();

  SystemReliabilityDiagnostics(const SystemReliabilityDiagnostics&) = delete;
  SystemReliabilityDiagnostics& operator=(const SystemReliabilityDiagnostics&) = delete;

  /**
   * @brief Enforce engineering-lab runtime gate (debugger + VM signature).
   */
  std::expected<void, std::string> EnsureEngineerEnvironment() const;

  /**
   * @brief Scan running processes via Toolhelp32 and return normalized metadata.
   */
  std::expected<std::vector<ProcessMetadata>, std::string> ScanProcesses() const;

  /**
   * @brief Collect memory and CPU metrics for a target PID.
   */
  std::expected<ProcessPerformance, std::string> CollectPerformance(DWORD pid);

  /**
   * @brief Return false for protected/critical process names and system-reserved PIDs.
   */
  bool CanRemediate(DWORD pid, const std::wstring& image_name) const;

  /**
   * @brief Attempt graceful process recovery using WM_CLOSE then terminate as last resort.
   */
  std::expected<void, std::string> AttemptGracefulRecovery(DWORD pid) const;

  /**
   * @brief Enumerate thread contexts for the process to help identify hang/zombie conditions.
   */
  std::expected<std::vector<ThreadDiagnostic>, std::string> DiagnoseThreads(DWORD pid) const;

  /**
   * @brief Cooperative monitoring loop entrypoint.
   */
  void RunMonitoringLoop(std::stop_token stop_token, DWORD interval_ms = 2000);

 private:
  std::expected<void, std::string> LogJsonEvent(const std::wstring& action,
                                                const std::wstring& details_json) const;

  std::optional<ProcessMetadata> FindProcessByPid(DWORD pid) const;

  HANDLE event_source_{nullptr};
};

}  // namespace win_ai_agent::kernel

#endif
