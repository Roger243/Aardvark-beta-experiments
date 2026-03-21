#include "src/core/firmware_persistence/SecureBootAuditBridge.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> SecureBootAuditBridge::BuildAudit() const {
  const auto state = reader_.ReadState();
  if (!state) return std::unexpected(state.error());
  return nlohmann::json{{"module", "SecureBoot_Audit_Bridge"},
                        {"secure_boot", state->secure_boot},
                        {"setup_mode", state->setup_mode},
                        {"read_only", true}};
}

}  // namespace win_ai_agent::nexus_pro
