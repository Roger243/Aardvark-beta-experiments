#pragma once

#include "src/core/global_shield/RemotePolicyTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class RemotePolicyFetcher {
 public:
  [[nodiscard]] std::expected<RemotePolicyDocument, WinErrorStruct> Fetch(const std::string& endpoint) const;
};

}  // namespace win_ai_agent::nexus_pro
