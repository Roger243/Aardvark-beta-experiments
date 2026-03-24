#pragma once

#include <string>
#include <vector>

namespace win_ai_agent::nexus_pro {

struct SnapshotInfo {
  std::string snapshot_id;
  std::string created_at;
};

struct RestorationRequest {
  std::string snapshot_id;
  std::string operator_confirmation_guid;
};

}  // namespace win_ai_agent::nexus_pro
