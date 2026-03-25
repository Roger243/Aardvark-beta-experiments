#pragma once

#include "src/core/global_shield/PeerDiscoveryTypes.hpp"

#include <vector>

namespace win_ai_agent::nexus_pro {

class MdnsPeerDiscovery {
 public:
  [[nodiscard]] std::vector<PeerNode> Discover() const;
};

}  // namespace win_ai_agent::nexus_pro
