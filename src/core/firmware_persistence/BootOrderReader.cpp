#include "src/core/firmware_persistence/BootOrderReader.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<BootOrderState, WinErrorStruct> BootOrderReader::ReadBootOrder() const {
#ifdef _WIN32
  std::uint16_t entries[64]{};
  const auto size = ::GetFirmwareEnvironmentVariableW(
      L"BootOrder", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}", entries, static_cast<DWORD>(sizeof(entries)));
  if (size == 0) return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW(BootOrder)"));
  BootOrderState state;
  const auto count = size / sizeof(std::uint16_t);
  state.entries.assign(entries, entries + count);
  return state;
#else
  return std::unexpected(MakeWinError("GetFirmwareEnvironmentVariableW(BootOrder)"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
