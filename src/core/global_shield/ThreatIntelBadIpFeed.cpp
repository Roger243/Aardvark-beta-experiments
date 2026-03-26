#include "src/core/global_shield/ThreatIntelBadIpFeed.hpp"

namespace win_ai_agent::nexus_pro {

nlohmann::json ThreatIntelBadIpFeed::ToJson(const ThreatIntelFeed& feed) const {
  return {{"source", feed.source}, {"bad_ips", feed.bad_ips}};
}

}  // namespace win_ai_agent::nexus_pro
