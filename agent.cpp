#include "network_manager.hpp"

#include <windows.h>
#include <psapi.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#pragma comment(lib, "psapi.lib")

namespace fs = std::filesystem;
using namespace std::chrono_literals;

namespace agent {

struct KernelMetrics final {
  double cpu_percent{};
  std::uint64_t memory_used_bytes{};
  std::uint64_t memory_total_bytes{};
  std::uint64_t io_read_bytes{};
  std::uint64_t io_write_bytes{};
};

struct OperationError final {
  std::string stage;
  std::string details;
  DWORD win32_error{};
};

class TempScriptFile final {
 public:
  TempScriptFile(const fs::path& root_dir, std::string_view content,
                 std::wstring extension = L".ps1") {
    fs::create_directories(root_dir);

    const auto tick =
        std::chrono::steady_clock::now().time_since_epoch().count();
    path_ = root_dir / (L"agent_task_" + std::to_wstring(tick) + extension);

    std::ofstream stream(path_, std::ios::binary);
    if (!stream) {
      throw std::runtime_error("Failed to create temp script file");
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.close();
  }

  TempScriptFile(const TempScriptFile&) = delete;
  TempScriptFile& operator=(const TempScriptFile&) = delete;

  TempScriptFile(TempScriptFile&&) = delete;
  TempScriptFile& operator=(TempScriptFile&&) = delete;

  ~TempScriptFile() {
    std::error_code ec;
    fs::remove(path_, ec);
  }

  [[nodiscard]] const fs::path& path() const noexcept { return path_; }

 private:
  fs::path path_;
};

[[nodiscard]] std::uint64_t FileTimeToUint64(const FILETIME& ft) {
  ULARGE_INTEGER value{};
  value.HighPart = ft.dwHighDateTime;
  value.LowPart = ft.dwLowDateTime;
  return value.QuadPart;
}

[[nodiscard]] KernelMetrics SampleMetrics() {
  static std::optional<std::array<FILETIME, 3>> previous_times;

  FILETIME idle{}, kernel{}, user{};
  if (!GetSystemTimes(&idle, &kernel, &user)) {
    throw std::runtime_error("GetSystemTimes failed");
  }

  MEMORYSTATUSEX mem{};
  mem.dwLength = sizeof(mem);
  if (!GlobalMemoryStatusEx(&mem)) {
    throw std::runtime_error("GlobalMemoryStatusEx failed");
  }

  IO_COUNTERS io{};
  if (!GetProcessIoCounters(GetCurrentProcess(), &io)) {
    throw std::runtime_error("GetProcessIoCounters failed");
  }

  double cpu = 0.0;
  if (previous_times.has_value()) {
    const auto prev_idle = FileTimeToUint64((*previous_times)[0]);
    const auto prev_kernel = FileTimeToUint64((*previous_times)[1]);
    const auto prev_user = FileTimeToUint64((*previous_times)[2]);

    const auto idle_diff = FileTimeToUint64(idle) - prev_idle;
    const auto kernel_diff = FileTimeToUint64(kernel) - prev_kernel;
    const auto user_diff = FileTimeToUint64(user) - prev_user;
    const auto busy = (kernel_diff + user_diff) - idle_diff;
    const auto total = kernel_diff + user_diff;

    if (total > 0) {
      cpu = (100.0 * static_cast<double>(busy)) / static_cast<double>(total);
    }
  }
  previous_times = std::array<FILETIME, 3>{idle, kernel, user};

  return KernelMetrics{
      .cpu_percent = cpu,
      .memory_used_bytes = mem.ullTotalPhys - mem.ullAvailPhys,
      .memory_total_bytes = mem.ullTotalPhys,
      .io_read_bytes = io.ReadTransferCount,
      .io_write_bytes = io.WriteTransferCount,
  };
}

[[nodiscard]] std::string BuildMetricsJson(const KernelMetrics& m) {
  std::ostringstream out;
  out << std::fixed << std::setprecision(2)
      << "{\"cpu\":" << m.cpu_percent
      << ",\"memory_used\":" << m.memory_used_bytes
      << ",\"memory_total\":" << m.memory_total_bytes
      << ",\"io_read\":" << m.io_read_bytes
      << ",\"io_write\":" << m.io_write_bytes << "}";
  return out.str();
}

[[nodiscard]] std::string BuildErrorJson(const OperationError& err) {
  std::ostringstream out;
  out << "{\"stage\":\"" << err.stage << "\",\"details\":\"" << err.details
      << "\",\"win32_error\":" << err.win32_error << "}";
  return out.str();
}

[[nodiscard]] std::string FetchSolution(NetworkManager& network) {
  // Safety model: fetch only declarative remediation plans, not executable code.
  const auto response = network.HttpGet(L"example.com", L"/agent/fix-plan.json");
  if (response.status_code != 200) {
    throw std::runtime_error("FetchSolution failed with non-200 status");
  }
  return response.body;
}

bool ExecuteTrustedPowerShellScript(const std::string& script) {
  // This function intentionally executes local, operator-provided scripts only.
  // Do NOT execute remote model output directly.
  const auto temp_root = fs::temp_directory_path() / "aardvark-agent";
  TempScriptFile temp(temp_root, script);

  std::wstring command =
      L"powershell.exe -NoProfile -ExecutionPolicy Bypass -File \"" +
      temp.path().wstring() + L"\"";

  STARTUPINFOW si{};
  si.cb = sizeof(si);

  PROCESS_INFORMATION pi{};
  std::vector<wchar_t> mutable_cmd(command.begin(), command.end());
  mutable_cmd.push_back(L'\0');

  if (!CreateProcessW(nullptr, mutable_cmd.data(), nullptr, nullptr, FALSE,
                      CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    return false;
  }

  CloseHandle(pi.hThread);
  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exit_code = 1;
  GetExitCodeProcess(pi.hProcess, &exit_code);
  CloseHandle(pi.hProcess);

  return exit_code == 0;
}

void StreamMetrics(NetworkManager& network, const std::wstring& host,
                   const std::wstring& path, int iterations = 10) {
  for (int i = 0; i < iterations; ++i) {
    const auto metrics = SampleMetrics();
    const auto payload = BuildMetricsJson(metrics);
    const auto response =
        network.HttpPost(host, path, payload, INTERNET_DEFAULT_HTTPS_PORT, true);

    std::cout << "[metrics] status=" << response.status_code << '\n';
    std::this_thread::sleep_for(1s);
  }
}

}  // namespace agent

int main() {
  try {
    agent::NetworkManager network;

    auto plan = agent::FetchSolution(network);
    std::cout << "Fetched remediation plan bytes: " << plan.size() << '\n';

    agent::StreamMetrics(network, L"example.com", L"/agent/metrics");

    // Example for controlled local execution (disabled by default):
    // const std::string script = "Write-Host 'trusted script'";
    // const bool ok = agent::ExecuteTrustedPowerShellScript(script);
    // std::cout << "Script status: " << ok << '\n';

    return 0;
  } catch (const std::exception& ex) {
    try {
      agent::NetworkManager network;
      const agent::OperationError err{.stage = "main",
                                      .details = ex.what(),
                                      .win32_error = GetLastError()};
      (void)network.HttpPost(L"example.com", L"/agent/errors",
                             agent::BuildErrorJson(err));
    } catch (...) {
      // Ignore secondary failure.
    }

    std::cerr << "Fatal error: " << ex.what() << '\n';
    return 1;
  }
}
