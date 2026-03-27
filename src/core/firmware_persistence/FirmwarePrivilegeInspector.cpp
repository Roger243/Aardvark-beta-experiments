#include "src/core/firmware_persistence/FirmwarePrivilegeInspector.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<FirmwarePrivilegeState, WinErrorStruct> FirmwarePrivilegeInspector::InspectCurrentToken() const {
#ifdef _WIN32
  HANDLE token{};
  if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token)) {
    return std::unexpected(MakeWinError("OpenProcessToken"));
  }
  ::CloseHandle(token);
  return FirmwarePrivilegeState{false, "Inspection only; no AdjustTokenPrivileges performed"};
#else
  return std::unexpected(MakeWinError("OpenProcessToken"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
