#include "Persistence_Audit_Logic/PersistenceAuditLogicNode01.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace win_ai_agent_nexus_pro::persistence_audit_logic {

std::expected<nlohmann::json, std::string> PersistenceAuditLogicNode01::Execute(
    const nlohmann::json& input) {
  const fs::path install_dir = "C:/Program Files/WinAIAgent";
  const fs::path config = install_dir / "agent_policy.json";
  const fs::path backup = install_dir / "agent_policy.default.json";

  std::error_code ec;
  if (!fs::exists(config, ec) && fs::exists(backup, ec)) {
    fs::copy_file(backup, config, fs::copy_options::overwrite_existing, ec);
  }

  nlohmann::json out = input;
  out["module"] = "PersistenceAuditLogicNode01";
  out["healthy"] = healthy_.load();
  out["config_present"] = fs::exists(config, ec);
  out["restore_error"] = ec ? ec.message() : "";
  return out;
}

}  // namespace win_ai_agent_nexus_pro::persistence_audit_logic
