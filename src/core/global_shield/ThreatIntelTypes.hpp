#pragma once

#include <string>
#include <vector>

namespace win_ai_agent::nexus_pro {

struct ThreatIntelFeed {
  std::string source;
  std::vector<std::string> bad_ips;
};

}  // namespace win_ai_agent::nexus_pro
