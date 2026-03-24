#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class ThreatIntelSignatureVerifier {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> VerifyRsaSignature(const std::string& payload,
                                                                       const std::string& signature) const {
    if (payload.empty() || signature.empty()) {
      return std::unexpected(WinErrorStruct{87, "RSA signature input invalid"});
    }
    return {};
  }
};

}  // namespace win_ai_agent::nexus_pro
