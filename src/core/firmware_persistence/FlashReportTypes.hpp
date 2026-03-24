#pragma once

#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

struct FlashIntegrityReport {
  nlohmann::json payload;
};

}  // namespace win_ai_agent::nexus_pro
