#include "WinAIAgent/Core/LocalSystemService.hpp"

#include <windows.h>

int wmain() {
  SERVICE_TABLE_ENTRYW dispatch_table[] = {
      {const_cast<LPWSTR>(win_ai_agent::core::LocalSystemService::kServiceName),
       win_ai_agent::core::LocalSystemService::ServiceMain},
      {nullptr, nullptr},
  };

  if (!StartServiceCtrlDispatcherW(dispatch_table)) {
    return static_cast<int>(GetLastError());
  }

  return 0;
}
