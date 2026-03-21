#pragma once

#include "src/core/global_shield/RemotePolicyTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class RemotePolicyVerifier {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> Verify(const RemotePolicyDocument& doc) const;
};

}  // namespace win_ai_agent::nexus_pro
