#include "src/core/firmware_persistence/NVRAMReadClient.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<NvramMetadataRecord, WinErrorStruct> NVRAMReadClient::ReadMetadata(
    const std::wstring& variable_name, const std::wstring& variable_guid) const {
#ifdef _WIN32
  unsigned char buffer[256]{};
  const auto size = ::GetFirmwareEnvironmentVariableW(variable_name.c_str(), variable_guid.c_str(), buffer,
                                                       static_cast<DWORD>(sizeof(buffer)));
  if (size == 0) {
    return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW"));
  }
  return NvramMetadataRecord{"UEFI:" + std::to_string(size), std::to_string(buffer[0]), true};
#else
  (void)variable_name;
  (void)variable_guid;
  return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW"));
#endif
}

std::expected<void, WinErrorStruct> NVRAMReadClient::WriteMetadataBlocked() const {
  return std::unexpected(WinErrorStruct{1314, "SetFirmwareEnvironmentVariableW blocked by zero-persistence policy"});
}

}  // namespace win_ai_agent::nexus_pro
