#include "WinAIAgent/Core/LocalSystemService.hpp"

#include <thread>

namespace win_ai_agent::core {

void WINAPI LocalSystemService::ServiceMain(DWORD argc, LPWSTR* argv) {
  (void)argc;
  (void)argv;

  status_handle_ = RegisterServiceCtrlHandlerExW(kServiceName, HandlerEx, nullptr);
  if (status_handle_ == nullptr) {
    return;
  }

  status_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  status_.dwServiceSpecificExitCode = 0;

  ReportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
  ReportStatus(SERVICE_RUNNING, NO_ERROR, 0);

  RunWorker();

  ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

DWORD WINAPI LocalSystemService::HandlerEx(DWORD control,
                                           DWORD event_type,
                                           void* event_data,
                                           void* context) {
  (void)event_type;
  (void)event_data;
  (void)context;

  if (control == SERVICE_CONTROL_STOP) {
    ReportStatus(SERVICE_STOP_PENDING, NO_ERROR, 2000);
    RequestStop();
    return NO_ERROR;
  }

  return ERROR_CALL_NOT_IMPLEMENTED;
}

void LocalSystemService::ReportStatus(DWORD current_state, DWORD win32_exit_code, DWORD wait_hint) {
  status_.dwCurrentState = current_state;
  status_.dwWin32ExitCode = win32_exit_code;
  status_.dwWaitHint = wait_hint;
  status_.dwControlsAccepted = current_state == SERVICE_RUNNING ? SERVICE_ACCEPT_STOP : 0;

  if (status_handle_ != nullptr) {
    SetServiceStatus(status_handle_, &status_);
  }
}

void LocalSystemService::RunWorker() {
  while (!stop_requested_.load()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void LocalSystemService::RequestStop() {
  stop_requested_.store(true);
}

}  // namespace win_ai_agent::core
