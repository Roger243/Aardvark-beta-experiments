#include "src/core/firmware_persistence/BootOrderVerifier.hpp"

#include <algorithm>

namespace win_ai_agent::nexus_pro {

bool BootOrderVerifier::IsRecoveryEntryPresent(const BootOrderState& state, std::uint16_t expected_entry) const {
  return std::find(state.entries.begin(), state.entries.end(), expected_entry) != state.entries.end();
}

}  // namespace win_ai_agent::nexus_pro
