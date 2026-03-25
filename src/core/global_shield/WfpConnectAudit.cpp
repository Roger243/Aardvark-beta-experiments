#include "src/core/global_shield/WfpConnectAudit.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> WfpConnectAudit::AuditConnectLayer() const {
  return nlohmann::json{{"module", "Network_Sentinel_WFP"},
                        {"layer", "FWPM_LAYER_ALE_AUTH_CONNECT_V4"},
                        {"mode", "audit-only"}};
}

}  // namespace win_ai_agent::nexus_pro
