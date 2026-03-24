#include "src/core/global_shield/EncryptedTunnelClient.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<TunnelSessionInfo, WinErrorStruct> EncryptedTunnelClient::Connect(const std::string& endpoint) const {
  return TunnelSessionInfo{endpoint, "TLS1.3"};
}

}  // namespace win_ai_agent::nexus_pro
