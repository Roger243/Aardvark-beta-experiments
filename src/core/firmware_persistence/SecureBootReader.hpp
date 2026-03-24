#pragma once

#include "src/core/firmware_persistence/SecureBootState.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>

namespace win_ai_agent::nexus_pro {

class SecureBootReader {
 public:
  [[nodiscard]] std::expected<SecureBootState, WinErrorStruct> ReadState() const;
};

}  // namespace win_ai_agent::nexus_pro
