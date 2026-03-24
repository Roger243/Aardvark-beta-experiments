#include "src/utils/WinApiTypes.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace win_ai_agent::nexus_pro {

WinErrorStruct MakeWinError(const char* context) noexcept {
#ifdef _WIN32
  const auto last = ::GetLastError();
  return WinErrorStruct{last, std::string(context) + " failed with Win32 error " + std::to_string(last)};
#else
  (void)context;
  return WinErrorStruct{1, "Win32 APIs are unavailable on this platform"};
#endif
}

}  // namespace win_ai_agent::nexus_pro
