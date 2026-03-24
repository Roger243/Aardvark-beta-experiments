#pragma once

#include "src/core/firmware_persistence/FlashReportTypes.hpp"

namespace win_ai_agent::nexus_pro {

class FlashJsonPackager {
 public:
  [[nodiscard]] FlashIntegrityReport Build(const nlohmann::json& nvram,
                                           const nlohmann::json& boot_order,
                                           const nlohmann::json& secure_boot,
                                           const nlohmann::json& privilege) const;
};

}  // namespace win_ai_agent::nexus_pro
