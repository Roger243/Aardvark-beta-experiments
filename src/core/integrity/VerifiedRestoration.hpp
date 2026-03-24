#pragma once

#include "src/core/integrity/VssTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class VerifiedRestoration {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> Restore(const RestorationRequest& request,
                                                            const std::string& required_guid) const;
};

}  // namespace win_ai_agent::nexus_pro
