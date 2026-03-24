#pragma once

#include "src/core/global_shield/ThreatIntelTypes.hpp"

#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class ThreatIntelBadIpFeed {
 public:
  [[nodiscard]] nlohmann::json ToJson(const ThreatIntelFeed& feed) const;
};

}  // namespace win_ai_agent::nexus_pro
