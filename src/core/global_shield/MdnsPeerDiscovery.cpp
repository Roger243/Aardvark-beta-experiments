#include "src/core/global_shield/MdnsPeerDiscovery.hpp"

namespace win_ai_agent::nexus_pro {

std::vector<PeerNode> MdnsPeerDiscovery::Discover() const {
  return {{"node-mdns.local", "mDNS"}};
}

}  // namespace win_ai_agent::nexus_pro
