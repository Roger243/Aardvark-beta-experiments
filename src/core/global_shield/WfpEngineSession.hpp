#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class WfpEngineSession {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> Open() const;
};

}  // namespace win_ai_agent::nexus_pro
