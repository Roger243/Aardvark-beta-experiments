#pragma once

#include "src/core/global_shield/WfpTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class WfpConnectAudit {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> AuditConnectLayer() const;
};

}  // namespace win_ai_agent::nexus_pro
