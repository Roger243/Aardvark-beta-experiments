#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace win_ai_agent::nexus_pro {

class UefiStateSerializer {
 public:
  [[nodiscard]] nlohmann::json BuildState(const std::string& secure_boot,
                                          const std::string& setup_mode,
                                          const std::string& audit_mode) const;
};

}  // namespace win_ai_agent::nexus_pro
