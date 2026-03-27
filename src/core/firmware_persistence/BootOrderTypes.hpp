#pragma once

#include <cstdint>
#include <vector>

namespace win_ai_agent::nexus_pro {

struct BootOrderState {
  std::vector<std::uint16_t> entries;
};

}  // namespace win_ai_agent::nexus_pro
