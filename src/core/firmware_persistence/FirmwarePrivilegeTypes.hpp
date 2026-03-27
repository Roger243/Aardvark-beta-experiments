#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

struct FirmwarePrivilegeState {
  bool has_system_environment_privilege{false};
  std::string detail;
};

}  // namespace win_ai_agent::nexus_pro
