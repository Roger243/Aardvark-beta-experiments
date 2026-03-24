#include "src/core/global_shield/WsaLookupDiscovery.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<std::vector<PeerNode>, WinErrorStruct> WsaLookupDiscovery::Discover() const {
  return std::vector<PeerNode>{{"node.local", "WSALookupService"}};
}

}  // namespace win_ai_agent::nexus_pro
