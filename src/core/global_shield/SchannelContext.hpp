#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class SchannelContext {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> Initialize() const;
};

}  // namespace win_ai_agent::nexus_pro
