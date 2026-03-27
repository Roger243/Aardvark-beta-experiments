#pragma once

#include "src/core/firmware_persistence/FirmwarePrivilegeTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class FirmwarePrivilegeInspector {
 public:
  [[nodiscard]] std::expected<FirmwarePrivilegeState, WinErrorStruct> InspectCurrentToken() const;
};

}  // namespace win_ai_agent::nexus_pro
