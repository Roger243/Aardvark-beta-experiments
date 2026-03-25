#include "src/core/integrity/UefiBaselineConfig.hpp"

#include <fstream>

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> UefiBaselineConfig::Load(const std::string& path) const {
  std::ifstream in(path);
  if (!in) {
    return std::unexpected(WinErrorStruct{2, "Unable to open UEFI baseline JSON"});
  }

  nlohmann::json baseline;
  in >> baseline;
  return baseline;
}

}  // namespace win_ai_agent::nexus_pro
