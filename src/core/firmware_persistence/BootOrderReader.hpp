#pragma once

#include "src/core/firmware_persistence/BootOrderTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class BootOrderReader {
 public:
  [[nodiscard]] std::expected<BootOrderState, WinErrorStruct> ReadBootOrder() const;
};

}  // namespace win_ai_agent::nexus_pro
