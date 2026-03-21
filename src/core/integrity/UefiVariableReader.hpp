#pragma once

#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::nexus_pro {

class UefiVariableReader {
 public:
  [[nodiscard]] std::expected<std::string, WinErrorStruct> ReadStringVariable(
      const std::wstring& name, const std::wstring& guid) const;
};

}  // namespace win_ai_agent::nexus_pro
