#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace win_ai_agent::nexus_pro {

class UefiBaselineConfig {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> Load(const std::string& path) const;
};

}  // namespace win_ai_agent::nexus_pro
