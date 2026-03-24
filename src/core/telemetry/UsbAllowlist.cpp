#include "src/core/telemetry/UsbAllowlist.hpp"

#include <fstream>

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> UsbAllowlist::LoadEncryptedAllowlist(const std::string& path) {
  std::ifstream in(path);
  if (!in) {
    return std::unexpected(WinErrorStruct{2, "Unable to load allowlist"});
  }

  nlohmann::json raw;
  in >> raw;
  for (const auto& id : raw.value("hardware_ids", nlohmann::json::array())) {
    allowed_ids_.insert(id.get<std::string>());
  }
  return {};
}

bool UsbAllowlist::IsAllowed(const std::string& hardware_id) const {
  return allowed_ids_.contains(hardware_id);
}

}  // namespace win_ai_agent::nexus_pro
