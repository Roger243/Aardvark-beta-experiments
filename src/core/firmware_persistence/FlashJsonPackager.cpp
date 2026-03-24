#include "src/core/firmware_persistence/FlashJsonPackager.hpp"

namespace win_ai_agent::nexus_pro {

FlashIntegrityReport FlashJsonPackager::Build(const nlohmann::json& nvram,
                                              const nlohmann::json& boot_order,
                                              const nlohmann::json& secure_boot,
                                              const nlohmann::json& privilege) const {
  return FlashIntegrityReport{{{"module", "Flash_Integrity_Report"},
                               {"nvram", nvram},
                               {"boot_order", boot_order},
                               {"secure_boot", secure_boot},
                               {"privilege", privilege},
                               {"threat_signal", "audit-only"}}};
}

}  // namespace win_ai_agent::nexus_pro
