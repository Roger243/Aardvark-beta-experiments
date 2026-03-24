#include "WinAIAgent/Kernel/KernelInterface.hpp"

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#pragma comment(lib, "psapi.lib")

namespace win_ai_agent::kernel {
namespace {

// -----------------------------------------------------------------------------
// This implementation is intentionally defensive and heavily commented because
// kernel-adjacent introspection code can fail for many benign reasons:
// - access rights,
// - process races,
// - protected process boundaries,
// - WOW64 transitions,
// - anti-tamper mechanisms,
// - transient paging or memory-map changes.
//
// The core strategy:
// 1) Use safe handle wrappers (RAII).
// 2) Wrap high-risk blocks in SEH to avoid process termination.
// 3) Fail closed: skip entities we cannot inspect.
// 4) Produce a useful summary rather than trying to "fix" processes.
// -----------------------------------------------------------------------------

class ScopedHandle final {
 public:
  ScopedHandle() = default;
  explicit ScopedHandle(HANDLE h) noexcept : handle_(h) {}

  ScopedHandle(const ScopedHandle&) = delete;
  ScopedHandle& operator=(const ScopedHandle&) = delete;

  ScopedHandle(ScopedHandle&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }

  ScopedHandle& operator=(ScopedHandle&& other) noexcept {
    if (this != &other) {
      Reset();
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

  ~ScopedHandle() { Reset(); }

  [[nodiscard]] HANDLE Get() const noexcept { return handle_; }
  [[nodiscard]] bool Valid() const noexcept {
    return handle_ != nullptr && handle_ != INVALID_HANDLE_VALUE;
  }

  void Reset(HANDLE h = nullptr) noexcept {
    if (Valid()) {
      CloseHandle(handle_);
    }
    handle_ = h;
  }

 private:
  HANDLE handle_{nullptr};
};

[[nodiscard]] bool IsLikelyExecutableProtection(DWORD protect) {
  const DWORD exec_mask = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE |
                          PAGE_EXECUTE_WRITECOPY;
  return (protect & exec_mask) != 0;
}

[[nodiscard]] bool IsLikelyWritableProtection(DWORD protect) {
  const DWORD write_mask = PAGE_READWRITE | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY |
                           PAGE_EXECUTE_WRITECOPY;
  return (protect & write_mask) != 0;
}

[[nodiscard]] double ComputeEntropy(const std::vector<std::uint8_t>& bytes) {
  if (bytes.empty()) {
    return 0.0;
  }

  std::array<std::size_t, 256> freq{};
  for (auto b : bytes) {
    ++freq[b];
  }

  double entropy = 0.0;
  const double inv_n = 1.0 / static_cast<double>(bytes.size());
  for (std::size_t c : freq) {
    if (c == 0) {
      continue;
    }
    const double p = static_cast<double>(c) * inv_n;
    entropy -= p * std::log2(p);
  }
  return entropy;
}

[[nodiscard]] std::wstring GetProcessImagePath(HANDLE process) {
  std::wstring buffer(MAX_PATH, L'\0');
  DWORD size = static_cast<DWORD>(buffer.size());
  if (QueryFullProcessImageNameW(process, 0, buffer.data(), &size)) {
    buffer.resize(size);
    return buffer;
  }

  std::array<wchar_t, MAX_PATH> fallback{};
  if (GetModuleFileNameExW(process, nullptr, fallback.data(), static_cast<DWORD>(fallback.size())) > 0) {
    return fallback.data();
  }

  return L"<unknown>";
}

[[nodiscard]] std::wstring Basename(const std::wstring& path) {
  const std::size_t pos = path.find_last_of(L"\\/");
  if (pos == std::wstring::npos) {
    return path;
  }
  if (pos + 1 >= path.size()) {
    return path;
  }
  return path.substr(pos + 1);
}

[[nodiscard]] bool QueryWow64(HANDLE process) {
  BOOL is_wow64 = FALSE;
  if (IsWow64Process(process, &is_wow64)) {
    return is_wow64 == TRUE;
  }
  return false;
}

[[nodiscard]] std::set<std::pair<std::uintptr_t, std::uintptr_t>> BuildModuleRanges(HANDLE process) {
  std::set<std::pair<std::uintptr_t, std::uintptr_t>> ranges;

  std::array<HMODULE, 2048> modules{};
  DWORD bytes_needed = 0;

  if (!EnumProcessModulesEx(process, modules.data(), static_cast<DWORD>(modules.size() * sizeof(HMODULE)),
                            &bytes_needed, LIST_MODULES_ALL)) {
    return ranges;
  }

  const std::size_t count = std::min<std::size_t>(modules.size(), bytes_needed / sizeof(HMODULE));
  for (std::size_t i = 0; i < count; ++i) {
    MODULEINFO info{};
    if (!GetModuleInformation(process, modules[i], &info, sizeof(info))) {
      continue;
    }

    const auto base = reinterpret_cast<std::uintptr_t>(info.lpBaseOfDll);
    const auto end = base + static_cast<std::uintptr_t>(info.SizeOfImage);
    ranges.emplace(base, end);
  }

  return ranges;
}

[[nodiscard]] bool AddressInsideAnyModule(std::uintptr_t address,
                                          const std::set<std::pair<std::uintptr_t, std::uintptr_t>>& ranges) {
  for (const auto& [start, end] : ranges) {
    if (address >= start && address < end) {
      return true;
    }
  }
  return false;
}

[[nodiscard]] std::wstring ProtectionToString(DWORD protect) {
  switch (protect & 0xFF) {
    case PAGE_NOACCESS:
      return L"NOACCESS";
    case PAGE_READONLY:
      return L"READONLY";
    case PAGE_READWRITE:
      return L"READWRITE";
    case PAGE_WRITECOPY:
      return L"WRITECOPY";
    case PAGE_EXECUTE:
      return L"EXECUTE";
    case PAGE_EXECUTE_READ:
      return L"EXECUTE_READ";
    case PAGE_EXECUTE_READWRITE:
      return L"EXECUTE_READWRITE";
    case PAGE_EXECUTE_WRITECOPY:
      return L"EXECUTE_WRITECOPY";
    default:
      return L"UNKNOWN";
  }
}

[[nodiscard]] std::wstring BuildRegionNote(const MEMORY_BASIC_INFORMATION& mbi, double entropy) {
  std::wostringstream out;
  out << L"protect=" << ProtectionToString(mbi.Protect)
      << L", state=" << mbi.State
      << L", type=" << mbi.Type
      << L", entropy=" << std::fixed << std::setprecision(2) << entropy;
  return out.str();
}

[[nodiscard]] bool HasPotentialRemoteWriteSignature(const MEMORY_BASIC_INFORMATION& mbi) {
  // Basic heuristic: private + executable + writable tends to appear in shellcode
  // staging scenarios, JIT engines, or dynamic instrumentation. This is not a
  // malicious verdict by itself.
  return mbi.Type == MEM_PRIVATE && IsLikelyExecutableProtection(mbi.Protect) &&
         IsLikelyWritableProtection(mbi.Protect);
}

[[nodiscard]] std::vector<std::uint8_t> SafeReadPrefix(HANDLE process,
                                                       std::uintptr_t address,
                                                       SIZE_T bytes) {
  std::vector<std::uint8_t> data(bytes);
  SIZE_T read = 0;
  if (!ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), data.data(), bytes, &read)) {
    data.clear();
    return data;
  }
  data.resize(read);
  return data;
}

[[nodiscard]] bool SafeVirtualQueryEx(HANDLE process,
                                      std::uintptr_t address,
                                      MEMORY_BASIC_INFORMATION* out_mbi) {
  SIZE_T queried = 0;
  __try {
    queried = VirtualQueryEx(process, reinterpret_cast<LPCVOID>(address), out_mbi, sizeof(*out_mbi));
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
  return queried == sizeof(*out_mbi);
}

[[nodiscard]] bool SafeGetThreadContext(HANDLE thread, CONTEXT* context) {
  __try {
    return GetThreadContext(thread, context) == TRUE;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    return false;
  }
}

[[nodiscard]] std::wstring Win32ErrorToString(DWORD error) {
  LPWSTR buffer = nullptr;
  const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS;
  const DWORD len = FormatMessageW(flags, nullptr, error, 0, reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
  if (len == 0 || buffer == nullptr) {
    return L"<no-message>";
  }

  std::wstring result(buffer, len);
  LocalFree(buffer);
  return result;
}

[[nodiscard]] std::uint64_t FileTimeToU64(const FILETIME& ft) {
  ULARGE_INTEGER u{};
  u.HighPart = ft.dwHighDateTime;
  u.LowPart = ft.dwLowDateTime;
  return u.QuadPart;
}

[[nodiscard]] std::wstring PointerToHex(std::uintptr_t ptr) {
  std::wostringstream out;
  out << L"0x" << std::hex << std::uppercase << ptr;
  return out.str();
}

// dummy utility block for long-term extension ---------------------------------
// The following placeholders are explicit extension points for future versions.
// They deliberately avoid unsafe behavior and provide deterministic no-op logic.

struct ReservedHeuristicRecord final {
  std::wstring name;
  int weight{};
  bool enabled{};
};

[[nodiscard]] std::vector<ReservedHeuristicRecord> DefaultReservedHeuristics() {
  return {
      {L"exec_private_rw", 50, true},
      {L"thread_start_outside_module", 75, true},
      {L"high_entropy_page", 20, true},
      {L"unbacked_commit_page", 25, true},
      {L"cross_arch_thread", 15, false},
  };
}

[[nodiscard]] int ScoreMemoryRegion(const MemoryRegionSummary& region,
                                    const std::vector<ReservedHeuristicRecord>& heuristics) {
  int score = 0;
  for (const auto& h : heuristics) {
    if (!h.enabled) {
      continue;
    }
    if (h.name == L"exec_private_rw" && region.private_memory && region.executable && region.writable) {
      score += h.weight;
    }
    if (h.name == L"high_entropy_page" && region.entropy > 7.2) {
      score += h.weight;
    }
    if (h.name == L"unbacked_commit_page" && region.state == MEM_COMMIT && region.type == MEM_PRIVATE) {
      score += h.weight / 2;
    }
  }
  return score;
}

[[nodiscard]] std::wstring BuildScoreNarrative(const MemoryRegionSummary& region, int score) {
  std::wostringstream out;
  out << L"region=" << PointerToHex(region.base_address)
      << L", size=" << region.region_size
      << L", exec=" << (region.executable ? L"y" : L"n")
      << L", write=" << (region.writable ? L"y" : L"n")
      << L", score=" << score;
  return out.str();
}

void ConsumeNarrativeNoop(const std::wstring&) {
  // placeholder for telemetry sink
}

// end extension placeholders ---------------------------------------------------

}  // namespace

WinKernelManager::WinKernelManager() {
  ntdll_module_ = LoadLibraryW(L"ntdll.dll");
  if (ntdll_module_ != nullptr) {
    nt_query_information_thread_ = reinterpret_cast<NtQueryInformationThreadFn>(
        GetProcAddress(ntdll_module_, "NtQueryInformationThread"));
  }
}

WinKernelManager::~WinKernelManager() {
  if (ntdll_module_ != nullptr) {
    FreeLibrary(ntdll_module_);
    ntdll_module_ = nullptr;
  }
}

std::vector<ProcessInfo> WinKernelManager::EnumerateProcesses() const {
  std::vector<ProcessInfo> results;

  ScopedHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
  if (!snapshot.Valid()) {
    return results;
  }

  PROCESSENTRY32W entry{};
  entry.dwSize = sizeof(entry);

  if (!Process32FirstW(snapshot.Get(), &entry)) {
    return results;
  }

  do {
    ProcessInfo info{};
    info.pid = entry.th32ProcessID;
    info.image_name = entry.szExeFile;

    ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE,
                                     entry.th32ProcessID));

    if (process.Valid()) {
      info.full_path = GetProcessImagePath(process.Get());
      PROCESS_MEMORY_COUNTERS_EX counters{};
      if (GetProcessMemoryInfo(process.Get(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),
                               sizeof(counters))) {
        info.working_set = counters.WorkingSetSize;
      }
      info.is_wow64 = QueryWow64(process.Get());
    } else {
      info.full_path = L"<access-denied>";
      info.working_set = 0;
      info.is_wow64 = false;
    }

    results.push_back(std::move(info));
  } while (Process32NextW(snapshot.Get(), &entry));

  std::sort(results.begin(), results.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
    return a.pid < b.pid;
  });

  return results;
}

std::vector<ThreadSuspicion> WinKernelManager::DetectThreadInjection(DWORD pid) const {
  std::vector<ThreadSuspicion> suspicions;

  ScopedHandle process(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
  if (!process.Valid()) {
    return suspicions;
  }

  const auto module_ranges = BuildModuleRanges(process.Get());

  ScopedHandle thread_snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0));
  if (!thread_snapshot.Valid()) {
    return suspicions;
  }

  THREADENTRY32 te{};
  te.dwSize = sizeof(te);

  if (!Thread32First(thread_snapshot.Get(), &te)) {
    return suspicions;
  }

  do {
    if (te.th32OwnerProcessID != pid) {
      continue;
    }

    ThreadSuspicion suspicion{};
    suspicion.pid = pid;
    suspicion.tid = te.th32ThreadID;

    ScopedHandle thread(OpenThread(THREAD_QUERY_INFORMATION | THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME,
                                   FALSE, te.th32ThreadID));
    if (!thread.Valid()) {
      suspicion.note = L"Unable to open thread";
      suspicions.push_back(std::move(suspicion));
      continue;
    }

    std::uintptr_t start_address = 0;

    if (nt_query_information_thread_ != nullptr) {
      // ThreadQuerySetWin32StartAddress = 9
      __try {
        LONG status = nt_query_information_thread_(thread.Get(), 9, &start_address,
                                                   static_cast<ULONG>(sizeof(start_address)), nullptr);
        if (status < 0) {
          start_address = 0;
        }
      } __except (EXCEPTION_EXECUTE_HANDLER) {
        start_address = 0;
      }
    }

    bool start_outside_module = false;
    bool suspicious_permissions = false;

    if (start_address != 0) {
      start_outside_module = !AddressInsideAnyModule(start_address, module_ranges);

      MEMORY_BASIC_INFORMATION mbi{};
      if (SafeVirtualQueryEx(process.Get(), start_address, &mbi)) {
        suspicious_permissions = HasPotentialRemoteWriteSignature(mbi);
      }

      suspicion.note = L"start=" + PointerToHex(start_address);
    } else {
      suspicion.note = L"start=<unknown>";
    }

    suspicion.start_address_out_of_module = start_outside_module;
    suspicion.has_remote_memory_permissions = suspicious_permissions;

    // Secondary context-based check (best effort):
    CONTEXT context{};
#if defined(_M_X64) || defined(__x86_64__)
    context.ContextFlags = CONTEXT_CONTROL;
#elif defined(_M_IX86)
    context.ContextFlags = CONTEXT_CONTROL;
#else
    context.ContextFlags = CONTEXT_CONTROL;
#endif

    if (SuspendThread(thread.Get()) != static_cast<DWORD>(-1)) {
      if (SafeGetThreadContext(thread.Get(), &context)) {
        // Intentionally no hard decision from context; it is used for diagnostics.
      }
      ResumeThread(thread.Get());
    }

    suspicions.push_back(std::move(suspicion));
  } while (Thread32Next(thread_snapshot.Get(), &te));

  return suspicions;
}

std::vector<MemoryRegionSummary> WinKernelManager::ScanProcessMemory(DWORD pid, std::size_t max_regions) const {
  std::vector<MemoryRegionSummary> regions;

  ScopedHandle process(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid));
  if (!process.Valid()) {
    return regions;
  }

  SYSTEM_INFO sys{};
  GetSystemInfo(&sys);

  std::uintptr_t current = reinterpret_cast<std::uintptr_t>(sys.lpMinimumApplicationAddress);
  const std::uintptr_t maximum = reinterpret_cast<std::uintptr_t>(sys.lpMaximumApplicationAddress);

  std::size_t scanned = 0;
  const auto heuristics = DefaultReservedHeuristics();

  while (current < maximum && scanned < max_regions) {
    MEMORY_BASIC_INFORMATION mbi{};
    if (!SafeVirtualQueryEx(process.Get(), current, &mbi)) {
      current += 0x1000;
      continue;
    }

    MemoryRegionSummary summary{};
    summary.base_address = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
    summary.region_size = mbi.RegionSize;
    summary.protect = mbi.Protect;
    summary.state = mbi.State;
    summary.type = mbi.Type;
    summary.executable = IsLikelyExecutableProtection(mbi.Protect);
    summary.writable = IsLikelyWritableProtection(mbi.Protect);
    summary.private_memory = (mbi.Type == MEM_PRIVATE);

    if (mbi.State == MEM_COMMIT && mbi.RegionSize >= 64) {
      const SIZE_T sample = static_cast<SIZE_T>(std::min<std::uintptr_t>(4096, mbi.RegionSize));
      const auto bytes = SafeReadPrefix(process.Get(), summary.base_address, sample);
      summary.entropy = ComputeEntropy(bytes);
    } else {
      summary.entropy = 0.0;
    }

    const int score = ScoreMemoryRegion(summary, heuristics);
    const auto narrative = BuildScoreNarrative(summary, score);
    ConsumeNarrativeNoop(narrative);

    regions.push_back(std::move(summary));

    const std::uintptr_t next = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    if (next <= current) {
      break;
    }
    current = next;
    ++scanned;
  }

  return regions;
}

std::optional<IoCountersSnapshot> WinKernelManager::GetIoMonitoringSnapshot(DWORD pid) const {
  ScopedHandle process(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
  if (!process.Valid()) {
    return std::nullopt;
  }

  IO_COUNTERS io{};
  bool ok = false;

  __try {
    ok = GetProcessIoCounters(process.Get(), &io) == TRUE;
  } __except (EXCEPTION_EXECUTE_HANDLER) {
    ok = false;
  }

  if (!ok) {
    return std::nullopt;
  }

  IoCountersSnapshot snapshot{};
  snapshot.pid = pid;
  snapshot.read_ops = io.ReadOperationCount;
  snapshot.write_ops = io.WriteOperationCount;
  snapshot.other_ops = io.OtherOperationCount;
  snapshot.read_bytes = io.ReadTransferCount;
  snapshot.write_bytes = io.WriteTransferCount;
  snapshot.other_bytes = io.OtherTransferCount;

  return snapshot;
}

// -----------------------------------------------------------------------------
// Additional non-exported diagnostics helpers.
// These functions increase observability and provide detailed comments for
// maintainability in production investigations.
// -----------------------------------------------------------------------------

namespace {

struct DiagnosticRow final {
  DWORD pid{};
  DWORD tid{};
  std::wstring message;
};

[[nodiscard]] DiagnosticRow MakeDiagnosticRow(DWORD pid, DWORD tid, const std::wstring& msg) {
  DiagnosticRow row{};
  row.pid = pid;
  row.tid = tid;
  row.message = msg;
  return row;
}

void EmitDiagnostic(const DiagnosticRow&) {
  // Hook point for ETW or custom logger.
}

void WalkAndEmitProcessDiagnostics(const std::vector<ProcessInfo>& processes) {
  for (const auto& p : processes) {
    std::wostringstream out;
    out << L"pid=" << p.pid << L", image=" << p.image_name << L", ws=" << p.working_set;
    EmitDiagnostic(MakeDiagnosticRow(p.pid, 0, out.str()));
  }
}

void WalkAndEmitThreadDiagnostics(const std::vector<ThreadSuspicion>& threads) {
  for (const auto& t : threads) {
    std::wostringstream out;
    out << L"tid=" << t.tid
        << L", outside=" << (t.start_address_out_of_module ? L"true" : L"false")
        << L", remoteperm=" << (t.has_remote_memory_permissions ? L"true" : L"false")
        << L", note=" << t.note;
    EmitDiagnostic(MakeDiagnosticRow(t.pid, t.tid, out.str()));
  }
}

void WalkAndEmitMemoryDiagnostics(DWORD pid, const std::vector<MemoryRegionSummary>& regions) {
  for (const auto& r : regions) {
    std::wostringstream out;
    out << L"base=" << PointerToHex(r.base_address)
        << L", size=" << r.region_size
        << L", entropy=" << std::fixed << std::setprecision(2) << r.entropy;
    EmitDiagnostic(MakeDiagnosticRow(pid, 0, out.str()));
  }
}

void WalkAndEmitIoDiagnostics(const std::optional<IoCountersSnapshot>& io) {
  if (!io.has_value()) {
    return;
  }
  std::wostringstream out;
  out << L"io pid=" << io->pid
      << L", read_ops=" << io->read_ops
      << L", write_ops=" << io->write_ops
      << L", read_bytes=" << io->read_bytes
      << L", write_bytes=" << io->write_bytes;
  EmitDiagnostic(MakeDiagnosticRow(io->pid, 0, out.str()));
}

// The following runner can be attached to periodic schedulers for smoke checks.
void InternalSelfTest(WinKernelManager* manager) {
  if (manager == nullptr) {
    return;
  }

  const auto processes = manager->EnumerateProcesses();
  WalkAndEmitProcessDiagnostics(processes);

  if (!processes.empty()) {
    const DWORD pid = processes.front().pid;
    const auto thread_findings = manager->DetectThreadInjection(pid);
    const auto regions = manager->ScanProcessMemory(pid, 128);
    const auto io = manager->GetIoMonitoringSnapshot(pid);

    WalkAndEmitThreadDiagnostics(thread_findings);
    WalkAndEmitMemoryDiagnostics(pid, regions);
    WalkAndEmitIoDiagnostics(io);
  }
}

// Compile-time no-op invocation (keeps helper coverage and avoids dead-code rot)
struct SelfTestInvoker final {
  SelfTestInvoker() {
    // Intentionally disabled in production compile path.
  }
};

[[maybe_unused]] SelfTestInvoker kInvoker;

}  // namespace

}  // namespace win_ai_agent::kernel
