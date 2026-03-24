#include "src/core/integrity/UefiVariableReader.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<std::string, WinErrorStruct> UefiVariableReader::ReadStringVariable(
    const std::wstring& name, const std::wstring& guid) const {
#ifdef _WIN32
  unsigned char value[8]{};
  const auto bytes = ::GetFirmwareEnvironmentVariableW(name.c_str(), guid.c_str(), value, sizeof(value));
  if (bytes == 0) {
    return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW"));
  }
  return std::to_string(static_cast<unsigned int>(value[0]));
#else
  (void)name;
  (void)guid;
  return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
