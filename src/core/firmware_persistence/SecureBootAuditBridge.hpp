#pragma once

#include "src/core/firmware_persistence/SecureBootReader.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class SecureBootAuditBridge {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> BuildAudit() const;

 private:
  SecureBootReader reader_;
};

}  // namespace win_ai_agent::nexus_pro
