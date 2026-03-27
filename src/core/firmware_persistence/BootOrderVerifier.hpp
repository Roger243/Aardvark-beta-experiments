#pragma once

#include "src/core/firmware_persistence/BootOrderTypes.hpp"

namespace win_ai_agent::nexus_pro {

class BootOrderVerifier {
 public:
  [[nodiscard]] bool IsRecoveryEntryPresent(const BootOrderState& state, std::uint16_t expected_entry) const;
};

}  // namespace win_ai_agent::nexus_pro
