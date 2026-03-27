#pragma once

#include "src/core/global_shield/PeerDiscoveryTypes.hpp"

#include <nlohmann/json.hpp>
#include <vector>

namespace win_ai_agent::nexus_pro {

class PeerTelemetryMerger {
 public:
  [[nodiscard]] nlohmann::json Merge(const std::vector<PeerNode>& nodes) const {
    return {{"peer_count", nodes.size()}};
  }
};

}  // namespace win_ai_agent::nexus_pro
