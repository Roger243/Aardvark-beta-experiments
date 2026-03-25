#include "WinAIAgent/Kernel/SystemReliabilityDiagnostics.hpp"

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cwctype>
#include <expected>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

namespace win_ai_agent::kernel {
namespace {

struct HandleCloser final {
  void operator()(HANDLE handle) const noexcept {
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
      CloseHandle(handle);
    }
  }
};

using unique_handle = std::unique_ptr<void, HandleCloser>;

[[nodiscard]] unique_handle MakeHandle(HANDLE handle) {
  return unique_handle(handle);
}

[[nodiscard]] std::uint64_t FileTimeToU64(const FILETIME& ft) {
  ULARGE_INTEGER v{};
  v.HighPart = ft.dwHighDateTime;
  v.LowPart = ft.dwLowDateTime;
  return v.QuadPart;
}

[[nodiscard]] bool RegistryHasVmSignature() {
  HKEY key = nullptr;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\PCI", 0,
                    KEY_READ, &key) != ERROR_SUCCESS) {
    return false;
  }

  DWORD index = 0;
  wchar_t name[512];
  DWORD name_len = static_cast<DWORD>(std::size(name));
  bool found = false;

  while (RegEnumKeyExW(key, index, name, &name_len, nullptr, nullptr, nullptr, nullptr) ==
         ERROR_SUCCESS) {
    std::wstring upper(name, name_len);
    for (auto& ch : upper) {
      ch = static_cast<wchar_t>(std::towupper(ch));
    }

    if (upper.find(L"VMWARE") != std::wstring::npos ||
        upper.find(L"VIRTUALBOX") != std::wstring::npos ||
        upper.find(L"VBOX") != std::wstring::npos ||
        upper.find(L"HYPER-V") != std::wstring::npos) {
      found = true;
      break;
    }

    ++index;
    name_len = static_cast<DWORD>(std::size(name));
  }

  RegCloseKey(key);
  return found;
}

BOOL CALLBACK SendCloseToPidWindows(HWND hwnd, LPARAM lparam) {
  DWORD window_pid = 0;
  GetWindowThreadProcessId(hwnd, &window_pid);

  const DWORD target = static_cast<DWORD>(lparam);
  if (window_pid == target) {
    PostMessageW(hwnd, WM_CLOSE, 0, 0);
  }
  return TRUE;
}

}  // namespace

SystemReliabilityDiagnostics::SystemReliabilityDiagnostics() {
  event_source_ = RegisterEventSourceW(nullptr, L"WinAIAgent");
}

SystemReliabilityDiagnostics::~SystemReliabilityDiagnostics() {
  if (event_source_ != nullptr) {
    DeregisterEventSource(event_source_);
    event_source_ = nullptr;
  }
}

std::expected<void, std::string> SystemReliabilityDiagnostics::EnsureEngineerEnvironment() const {
  const bool debugger = IsDebuggerPresent() == TRUE;
  const bool vm_signature = RegistryHasVmSignature();

  if (!debugger || !vm_signature) {
    return std::unexpected(
        "SystemReliabilityDiagnostics aborted: debugger + VM signature required");
  }
  return {};
}

std::expected<std::vector<ProcessMetadata>, std::string>
SystemReliabilityDiagnostics::ScanProcesses() const {
  auto snapshot = MakeHandle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
  if (!snapshot || snapshot.get() == INVALID_HANDLE_VALUE) {
    return std::unexpected("CreateToolhelp32Snapshot failed");
  }

  PROCESSENTRY32W pe{};
  pe.dwSize = sizeof(pe);

  if (!Process32FirstW(static_cast<HANDLE>(snapshot.get()), &pe)) {
    return std::unexpected("Process32FirstW failed");
  }

  std::vector<ProcessMetadata> processes;
  do {
    processes.push_back(ProcessMetadata{
        .image_name = pe.szExeFile,
        .pid = pe.th32ProcessID,
        .thread_count = pe.cntThreads,
        .base_priority = pe.pcPriClassBase,
    });
  } while (Process32NextW(static_cast<HANDLE>(snapshot.get()), &pe));

  nlohmann::json j;
  j["action"] = "scan";
  j["process_count"] = processes.size();
  (void)LogJsonEvent(L"process_scan", std::wstring(j.dump().begin(), j.dump().end()));

  return processes;
}

std::expected<ProcessPerformance, std::string>
SystemReliabilityDiagnostics::CollectPerformance(DWORD pid) {
  auto process = MakeHandle(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE,
                                        pid));
  if (!process) {
    return std::unexpected("OpenProcess failed in CollectPerformance");
  }

  PROCESS_MEMORY_COUNTERS counters{};
  if (!GetProcessMemoryInfo(static_cast<HANDLE>(process.get()), &counters, sizeof(counters))) {
    return std::unexpected("GetProcessMemoryInfo failed");
  }

  FILETIME creation{}, exit{}, kernel{}, user{};
  if (!GetProcessTimes(static_cast<HANDLE>(process.get()), &creation, &exit, &kernel, &user)) {
    return std::unexpected("GetProcessTimes failed");
  }

  static std::unordered_map<DWORD, std::pair<std::uint64_t, std::chrono::steady_clock::time_point>>
      cpu_state;

  const std::uint64_t total = FileTimeToU64(kernel) + FileTimeToU64(user);
  const auto now = std::chrono::steady_clock::now();
  double cpu_percent = 0.0;

  auto it = cpu_state.find(pid);
  if (it != cpu_state.end()) {
    const auto elapsed_100ns = static_cast<double>(total - it->second.first);
    const auto elapsed_wall_ns =
        static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(now - it->second.second)
                                .count());

    if (elapsed_wall_ns > 0.0) {
      cpu_percent = (elapsed_100ns * 100.0) / (elapsed_wall_ns / 100.0);
    }
  }

  cpu_state[pid] = {total, now};

  return ProcessPerformance{
      .pid = pid,
      .working_set_size = counters.WorkingSetSize,
      .peak_working_set_size = counters.PeakWorkingSetSize,
      .page_fault_count = counters.PageFaultCount,
      .cpu_percent = cpu_percent,
  };
}

bool SystemReliabilityDiagnostics::CanRemediate(DWORD pid,
                                                const std::wstring& image_name) const {
  static const std::set<std::wstring> kBlocklist = {
      L"SYSTEM", L"IDLE", L"LSASS.EXE", L"CSRSS.EXE", L"SERVICES.EXE", L"WININIT.EXE", L"SMSS.EXE"};

  if (pid <= 4) {
    return false;
  }

  std::wstring upper = image_name;
  for (auto& c : upper) {
    c = static_cast<wchar_t>(std::towupper(c));
  }

  const bool allowed = !kBlocklist.contains(upper);
  if (!allowed) {
    nlohmann::json j;
    j["action"] = "filter_block";
    j["pid"] = pid;
    j["image"] = std::string(upper.begin(), upper.end());
    (void)LogJsonEvent(L"filter_block", std::wstring(j.dump().begin(), j.dump().end()));
  }
  return allowed;
}

std::expected<void, std::string> SystemReliabilityDiagnostics::AttemptGracefulRecovery(
    DWORD pid) const {
  auto metadata = FindProcessByPid(pid);
  if (!metadata.has_value()) {
    return std::unexpected("Process not found for remediation");
  }

  if (!CanRemediate(pid, metadata->image_name)) {
    return std::unexpected("Remediation blocked by safety filter");
  }

  EnumWindows(SendCloseToPidWindows, static_cast<LPARAM>(pid));

  auto process = MakeHandle(OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, pid));
  if (!process) {
    return std::unexpected("OpenProcess failed in AttemptGracefulRecovery");
  }

  const DWORD wait = WaitForSingleObject(static_cast<HANDLE>(process.get()), 5000);
  if (wait == WAIT_TIMEOUT) {
    if (!TerminateProcess(static_cast<HANDLE>(process.get()), 1)) {
      return std::unexpected("TerminateProcess failed after WM_CLOSE timeout");
    }

    nlohmann::json j;
    j["action"] = "recovery_terminate";
    j["pid"] = pid;
    (void)LogJsonEvent(L"recovery_terminate", std::wstring(j.dump().begin(), j.dump().end()));
  } else {
    nlohmann::json j;
    j["action"] = "recovery_graceful";
    j["pid"] = pid;
    (void)LogJsonEvent(L"recovery_graceful", std::wstring(j.dump().begin(), j.dump().end()));
  }

  return {};
}

std::expected<std::vector<ThreadDiagnostic>, std::string>
SystemReliabilityDiagnostics::DiagnoseThreads(DWORD pid) const {
  auto snapshot = MakeHandle(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));
  if (!snapshot || snapshot.get() == INVALID_HANDLE_VALUE) {
    return std::unexpected("CreateToolhelp32Snapshot failed for threads");
  }

  THREADENTRY32 te{};
  te.dwSize = sizeof(te);
  if (!Thread32First(static_cast<HANDLE>(snapshot.get()), &te)) {
    return std::unexpected("Thread32First failed");
  }

  std::vector<ThreadDiagnostic> rows;
  do {
    if (te.th32OwnerProcessID != pid) {
      continue;
    }

    auto thread = MakeHandle(OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT |
                                            THREAD_QUERY_INFORMATION,
                                        FALSE, te.th32ThreadID));
    if (!thread) {
      rows.push_back(ThreadDiagnostic{.pid = pid,
                                      .tid = te.th32ThreadID,
                                      .context_available = false,
                                      .instruction_pointer = 0});
      continue;
    }

    CONTEXT context{};
    context.ContextFlags = CONTEXT_CONTROL;

    std::uintptr_t ip = 0;
    bool ok = false;
    if (SuspendThread(static_cast<HANDLE>(thread.get())) != static_cast<DWORD>(-1)) {
      ok = GetThreadContext(static_cast<HANDLE>(thread.get()), &context) == TRUE;
#if defined(_M_X64) || defined(__x86_64__)
      ip = static_cast<std::uintptr_t>(context.Rip);
#elif defined(_M_IX86)
      ip = static_cast<std::uintptr_t>(context.Eip);
#endif
      ResumeThread(static_cast<HANDLE>(thread.get()));
    }

    rows.push_back(ThreadDiagnostic{.pid = pid,
                                    .tid = te.th32ThreadID,
                                    .context_available = ok,
                                    .instruction_pointer = ip});
  } while (Thread32Next(static_cast<HANDLE>(snapshot.get()), &te));

  return rows;
}

void SystemReliabilityDiagnostics::RunMonitoringLoop(std::stop_token stop_token,
                                                     DWORD interval_ms) {
  if (!EnsureEngineerEnvironment()) {
    return;
  }

  while (!stop_token.stop_requested()) {
    auto scan = ScanProcesses();
    if (scan) {
      for (const auto& proc : *scan) {
        (void)CollectPerformance(proc.pid);
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
  }
}

std::expected<void, std::string> SystemReliabilityDiagnostics::LogJsonEvent(
    const std::wstring& action, const std::wstring& details_json) const {
  if (event_source_ == nullptr) {
    return std::unexpected("Event source is not initialized");
  }

  const std::wstring message = L"{\"action\":\"" + action + L"\",\"details\":" + details_json +
                               L"}";

  LPCWSTR strings[1] = {message.c_str()};
  if (!ReportEventW(event_source_, EVENTLOG_INFORMATION_TYPE, 0, 0x1000, nullptr, 1, 0,
                    strings, nullptr)) {
    return std::unexpected("ReportEventW failed");
  }
  return {};
}

std::optional<ProcessMetadata> SystemReliabilityDiagnostics::FindProcessByPid(DWORD pid) const {
  auto all = ScanProcesses();
  if (!all) {
    return std::nullopt;
  }

  auto it = std::find_if(all->begin(), all->end(), [pid](const ProcessMetadata& p) {
    return p.pid == pid;
  });

  if (it == all->end()) {
    return std::nullopt;
  }
  return *it;
}

}  // namespace win_ai_agent::kernel
