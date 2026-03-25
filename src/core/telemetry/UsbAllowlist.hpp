#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_set>

namespace win_ai_agent::nexus_pro {

class UsbAllowlist {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> LoadEncryptedAllowlist(const std::string& path);
  [[nodiscard]] bool IsAllowed(const std::string& hardware_id) const;

 private:
  std::unordered_set<std::string> allowed_ids_;
};

}  // namespace win_ai_agent::nexus_pro
