#include "src/core/integrity/UefiStateSerializer.hpp"

namespace win_ai_agent::nexus_pro {

nlohmann::json UefiStateSerializer::BuildState(const std::string& secure_boot,
                                               const std::string& setup_mode,
                                               const std::string& audit_mode) const {
  return {
      {"SecureBoot", secure_boot},
      {"SetupMode", setup_mode},
      {"AuditMode", audit_mode},
  };
}

}  // namespace win_ai_agent::nexus_pro
