#pragma once

#include "src/core/global_shield/PeerDiscoveryTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <vector>

namespace win_ai_agent::nexus_pro {

class WsaLookupDiscovery {
 public:
  [[nodiscard]] std::expected<std::vector<PeerNode>, WinErrorStruct> Discover() const;
};

}  // namespace win_ai_agent::nexus_pro
