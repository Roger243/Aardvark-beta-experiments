#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

class NVRAMGuidProvider {
 public:
  [[nodiscard]] std::wstring AgentNamespaceGuid() const;
};

}  // namespace win_ai_agent::nexus_pro
