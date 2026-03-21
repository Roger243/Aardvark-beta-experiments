#ifndef WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_REGTRANSACTION_SAFE_HPP
#define WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_REGTRANSACTION_SAFE_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::registry_subsystem {

/**
 * @brief Stub interface for RegTransaction_Safe.
 */
class RegTransaction_Safe final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::registry_subsystem

#endif
