#pragma once

#include "src/core/integrity/BootIntegrityTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <vector>

namespace win_ai_agent::nexus_pro {

class BcdStoreReader {
 public:
  [[nodiscard]] std::expected<std::vector<BcdElementRecord>, WinErrorStruct> ReadBootManagerElements() const;
};

}  // namespace win_ai_agent::nexus_pro
