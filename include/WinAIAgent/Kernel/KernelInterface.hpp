#ifndef WIN_AI_AGENT_KERNEL_KERNEL_INTERFACE_HPP
#define WIN_AI_AGENT_KERNEL_KERNEL_INTERFACE_HPP

#include <windows.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace win_ai_agent::kernel {

struct ProcessInfo final {
  DWORD pid{};
  std::wstring image_name;
  std::wstring full_path;
  SIZE_T working_set{};
  bool is_wow64{};
};

struct ThreadSuspicion final {
  DWORD pid{};
  DWORD tid{};
  bool start_address_out_of_module{};
  bool has_remote_memory_permissions{};
  std::wstring note;
};

struct MemoryRegionSummary final {
  std::uintptr_t base_address{};
  SIZE_T region_size{};
  DWORD protect{};
  DWORD state{};
  DWORD type{};
  double entropy{};
  bool executable{};
  bool writable{};
  bool private_memory{};
};

struct IoCountersSnapshot final {
  DWORD pid{};
  std::uint64_t read_ops{};
  std::uint64_t write_ops{};
  std::uint64_t other_ops{};
  std::uint64_t read_bytes{};
  std::uint64_t write_bytes{};
  std::uint64_t other_bytes{};
};

class WinKernelManager final {
 public:
  WinKernelManager();
  ~WinKernelManager();

  WinKernelManager(const WinKernelManager&) = delete;
  WinKernelManager& operator=(const WinKernelManager&) = delete;

  std::vector<ProcessInfo> EnumerateProcesses() const;
  std::vector<ThreadSuspicion> DetectThreadInjection(DWORD pid) const;
  std::vector<MemoryRegionSummary> ScanProcessMemory(DWORD pid, std::size_t max_regions = 2048) const;
  std::optional<IoCountersSnapshot> GetIoMonitoringSnapshot(DWORD pid) const;

 private:
  using NtQueryInformationThreadFn = LONG (NTAPI*)(HANDLE, ULONG, PVOID, ULONG, PULONG);

  HMODULE ntdll_module_{};
  NtQueryInformationThreadFn nt_query_information_thread_{};
};

}  // namespace win_ai_agent::kernel

#endif  // WIN_AI_AGENT_KERNEL_KERNEL_INTERFACE_HPP
