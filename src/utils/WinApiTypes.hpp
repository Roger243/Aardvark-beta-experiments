#pragma once

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

struct WinErrorStruct {
  unsigned long code{0};
  std::string message;
};

[[nodiscard]] WinErrorStruct MakeWinError(const char* context) noexcept;

}  // namespace win_ai_agent::nexus_pro
