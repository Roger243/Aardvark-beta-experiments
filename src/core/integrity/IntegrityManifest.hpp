#pragma once

#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class IntegrityManifest {
 public:
  [[nodiscard]] static nlohmann::json CreateEmpty();
};

}  // namespace win_ai_agent::nexus_pro
