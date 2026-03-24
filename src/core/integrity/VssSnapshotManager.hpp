#pragma once

#include "src/core/integrity/VssTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>
#include <vector>

namespace win_ai_agent::nexus_pro {

class VssSnapshotManager {
 public:
  [[nodiscard]] std::expected<SnapshotInfo, WinErrorStruct> CreatePreRemediationSnapshot(
      const std::wstring& volume_name) const;
  [[nodiscard]] std::expected<std::vector<SnapshotInfo>, WinErrorStruct> CleanupOlderThan24h() const;
};

}  // namespace win_ai_agent::nexus_pro
