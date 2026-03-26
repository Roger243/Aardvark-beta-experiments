#pragma once

#include "src/core/global_shield/SchannelTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class EncryptedTunnelClient {
 public:
  [[nodiscard]] std::expected<TunnelSessionInfo, WinErrorStruct> Connect(const std::string& endpoint) const;
};

}  // namespace win_ai_agent::nexus_pro
