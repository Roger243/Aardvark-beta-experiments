#include "src/core/integrity/IntegrityManifest.hpp"

namespace win_ai_agent::nexus_pro {

nlohmann::json IntegrityManifest::CreateEmpty() {
  return {{"schema", "IntegrityManifest.json"}, {"anomaly_score", 0.0}, {"modules", nlohmann::json::array()}};
}

}  // namespace win_ai_agent::nexus_pro
