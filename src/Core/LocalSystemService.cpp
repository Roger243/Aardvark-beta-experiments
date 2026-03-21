#include "WinAIAgent/Core/LocalSystemService.hpp"

#include <sddl.h>

#include <chrono>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace win_ai_agent::core {
namespace {

std::wstring LookupPrivilegeNameFromLuid(const LUID& luid) {
  DWORD name_len = 0;
  LookupPrivilegeNameW(nullptr, const_cast<LUID*>(&luid), nullptr, &name_len);

  std::wstring name(name_len, L'\0');
  if (!LookupPrivilegeNameW(nullptr, const_cast<LUID*>(&luid), name.data(), &name_len)) {
    return L"<unknown_privilege>";
  }

  name.resize(name_len);
  return name;
}

}  // namespace

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
  LogTokenDiagnostics();
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

void LocalSystemService::LogTokenDiagnostics() {
  std::wofstream out(L"WinAIAgentServiceDiagnostics.log", std::ios::app);
  if (!out) {
    return;
  }

  HANDLE token = nullptr;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
    out << L"OpenProcessToken failed: " << GetLastError() << L"\n";
    return;
  }

  std::vector<std::byte> integrity_buffer(512);
  DWORD needed = 0;
  if (GetTokenInformation(token, TokenIntegrityLevel, integrity_buffer.data(),
                          static_cast<DWORD>(integrity_buffer.size()), &needed)) {
    const auto* til = reinterpret_cast<const TOKEN_MANDATORY_LABEL*>(integrity_buffer.data());
    const auto rid = *GetSidSubAuthority(til->Label.Sid,
                                         static_cast<DWORD>(*GetSidSubAuthorityCount(til->Label.Sid) - 1));
    out << L"IntegrityLevel=" << GetIntegrityLevelLabel(rid) << L" (RID=" << rid << L")\n";
  } else {
    out << L"GetTokenInformation(TokenIntegrityLevel) failed: " << GetLastError() << L"\n";
  }

  GetTokenInformation(token, TokenPrivileges, nullptr, 0, &needed);
  std::vector<std::byte> priv_buffer(needed);
  if (GetTokenInformation(token, TokenPrivileges, priv_buffer.data(), needed, &needed)) {
    const auto* privs = reinterpret_cast<const TOKEN_PRIVILEGES*>(priv_buffer.data());
    out << L"Privileges(" << privs->PrivilegeCount << L"):\n";
    for (DWORD i = 0; i < privs->PrivilegeCount; ++i) {
      const auto& entry = privs->Privileges[i];
      const auto name = LookupPrivilegeNameFromLuid(entry.Luid);
      out << L"  - " << name;
      if (entry.Attributes & SE_PRIVILEGE_ENABLED) {
        out << L" [enabled]";
      }
      out << L"\n";
    }
  } else {
    out << L"GetTokenInformation(TokenPrivileges) failed: " << GetLastError() << L"\n";
  }

  out << L"---\n";
  CloseHandle(token);
}

std::wstring LocalSystemService::GetIntegrityLevelLabel(DWORD rid) {
  if (rid >= SECURITY_MANDATORY_SYSTEM_RID) return L"System";
  if (rid >= SECURITY_MANDATORY_HIGH_RID) return L"High";
  if (rid >= SECURITY_MANDATORY_MEDIUM_RID) return L"Medium";
  if (rid >= SECURITY_MANDATORY_LOW_RID) return L"Low";
  return L"Untrusted";
}

}  // namespace win_ai_agent::core
