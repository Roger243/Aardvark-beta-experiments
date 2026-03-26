#include "src/core/integrity/VssSnapshotManager.hpp"

#ifdef _WIN32
#include <vss.h>
#include <vsbackup.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<SnapshotInfo, WinErrorStruct> VssSnapshotManager::CreatePreRemediationSnapshot(
    const std::wstring& volume_name) const {
#ifdef _WIN32
  (void)volume_name;
  // Placeholder: full IVssBackupComponents flow goes here.
  return SnapshotInfo{"00000000-0000-0000-0000-000000000001", "pre-remediation"};
#else
  (void)volume_name;
  return std::unexpected(MakeWinError("IVssBackupComponents::InitializeForBackup"));
#endif
}

std::expected<std::vector<SnapshotInfo>, WinErrorStruct> VssSnapshotManager::CleanupOlderThan24h() const {
#ifdef _WIN32
  return std::vector<SnapshotInfo>{};
#else
  return std::unexpected(MakeWinError("IVssBackupComponents::Query"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
