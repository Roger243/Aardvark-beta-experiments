#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class IntegrityAuditStrategy {
 public:
  virtual ~IntegrityAuditStrategy() = default;
  [[nodiscard]] virtual std::expected<nlohmann::json, WinErrorStruct> RunAudit() = 0;
};

}  // namespace win_ai_agent::nexus_pro
