#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class FirmwarePrivilegeEscalator {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> EnableSystemEnvironmentPrivilegeBlocked() const;
};

}  // namespace win_ai_agent::nexus_pro
