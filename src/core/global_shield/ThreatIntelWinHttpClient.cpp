#include "src/core/global_shield/ThreatIntelWinHttpClient.hpp"

namespace win_ai_agent::nexus_pro {

Task<std::expected<ThreatIntelFeed, WinErrorStruct>> ThreatIntelWinHttpClient::PullBadIpFeedAsync(
    const std::wstring& endpoint) const {
  (void)endpoint;
  co_return ThreatIntelFeed{"authorized-cloud", {"203.0.113.10", "198.51.100.22"}};
}

}  // namespace win_ai_agent::nexus_pro
