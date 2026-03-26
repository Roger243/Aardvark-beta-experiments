#include "src/core/global_shield/RemotePolicyVerifier.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> RemotePolicyVerifier::Verify(const RemotePolicyDocument& doc) const {
  if (doc.rsa_signature.empty()) {
    return std::unexpected(WinErrorStruct{13, "Remote policy missing RSA signature"});
  }
  return {};
}

}  // namespace win_ai_agent::nexus_pro
