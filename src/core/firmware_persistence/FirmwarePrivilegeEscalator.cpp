#include "src/core/firmware_persistence/FirmwarePrivilegeEscalator.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> FirmwarePrivilegeEscalator::EnableSystemEnvironmentPrivilegeBlocked() const {
  return std::unexpected(WinErrorStruct{1314, "ERROR_PRIVILEGE_NOT_HELD: privilege escalation blocked by policy"});
}

}  // namespace win_ai_agent::nexus_pro
