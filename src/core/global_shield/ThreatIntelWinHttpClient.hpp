#pragma once

#include "src/core/global_shield/ThreatIntelTypes.hpp"
#include "src/core/global_shield/WfpCoroutineTask.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class ThreatIntelWinHttpClient {
 public:
  [[nodiscard]] Task<std::expected<ThreatIntelFeed, WinErrorStruct>> PullBadIpFeedAsync(
      const std::wstring& endpoint) const;
};

}  // namespace win_ai_agent::nexus_pro
