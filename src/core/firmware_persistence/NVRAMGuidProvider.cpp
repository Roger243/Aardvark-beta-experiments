#include "src/core/firmware_persistence/NVRAMGuidProvider.hpp"

namespace win_ai_agent::nexus_pro {

std::wstring NVRAMGuidProvider::AgentNamespaceGuid() const {
  return L"{d49f5a90-91f8-4f63-b1a8-95f3f9de6f7a}";
}

}  // namespace win_ai_agent::nexus_pro
