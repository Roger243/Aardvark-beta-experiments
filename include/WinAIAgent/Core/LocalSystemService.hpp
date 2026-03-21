#pragma once

#include <windows.h>

#include <atomic>

namespace win_ai_agent::core {

class LocalSystemService final {
 public:
  static constexpr wchar_t kServiceName[] = L"WinAIAgentLocalSystemService";

  static void WINAPI ServiceMain(DWORD argc, LPWSTR* argv);
  static DWORD WINAPI HandlerEx(DWORD control, DWORD event_type, void* event_data, void* context);

 private:
  static void ReportStatus(DWORD current_state, DWORD win32_exit_code, DWORD wait_hint);
  static void RunWorker();
  static void RequestStop();

  static inline SERVICE_STATUS_HANDLE status_handle_ = nullptr;
  static inline SERVICE_STATUS status_{};
  static inline std::atomic<bool> stop_requested_{false};
};

}  // namespace win_ai_agent::core
