#include "src/core/global_shield/WfpEngineSession.hpp"

#ifdef _WIN32
#include <fwpmu.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> WfpEngineSession::Open() const {
#ifdef _WIN32
  HANDLE engine{};
  const auto status = ::FwpmEngineOpen0(nullptr, RPC_C_AUTHN_WINNT, nullptr, nullptr, &engine);
  if (status != ERROR_SUCCESS) {
    return std::unexpected(WinErrorStruct{status, "FwpmEngineOpen0 failed"});
  }
  ::FwpmEngineClose0(engine);
  return {};
#else
  return std::unexpected(MakeWinError("FwpmEngineOpen0"));
#endif
}

}  // namespace win_ai_agent::nexus_pro
