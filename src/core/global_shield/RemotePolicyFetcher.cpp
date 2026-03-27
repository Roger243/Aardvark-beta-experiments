#include "src/core/global_shield/RemotePolicyFetcher.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<RemotePolicyDocument, WinErrorStruct> RemotePolicyFetcher::Fetch(const std::string& endpoint) const {
  return RemotePolicyDocument{"{\"policy\":\"harden\"}", "rsa-signature-placeholder:" + endpoint};
}

}  // namespace win_ai_agent::nexus_pro
