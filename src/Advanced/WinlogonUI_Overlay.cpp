#include "WinAIAgent/Advanced/WinlogonUIOverlay.hpp"

#include <windows.h>
#include <mutex>

namespace win_ai_agent::advanced {

std::expected<void, std::string> WinlogonUIOverlay::ShowPreLogonStatus(const std::wstring& message) {
  std::unique_lock lock(mutex_);

  HDESK input = OpenInputDesktop(0, FALSE, DESKTOP_SWITCHDESKTOP | DESKTOP_READOBJECTS);
  if (input == nullptr) {
    return std::unexpected("OpenInputDesktop failed");
  }

  // Defensive design: do not switch to secure desktop or paint over Winlogon.
  // This function only surfaces a local status signal on the current interactive desktop.
  const int rc = MessageBoxW(nullptr, message.c_str(), L"AI Agent Status", MB_OK | MB_ICONINFORMATION);
  CloseDesktop(input);

  if (rc == 0) {
    return std::unexpected("MessageBoxW failed");
  }
  return {};
}

}  // namespace win_ai_agent::advanced
