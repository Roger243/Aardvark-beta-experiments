#pragma once

#include <cstdint>
#include <string>

namespace win_ai_agent::nexus_pro {

class BcdValueConverter {
 public:
  [[nodiscard]] static std::string ElementTypeToString(std::uint32_t type);
  [[nodiscard]] static std::string FlagValueToString(std::uint64_t value);
};

}  // namespace win_ai_agent::nexus_pro
