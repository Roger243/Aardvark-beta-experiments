#ifndef WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_REGBACKUP_MANAGER_HPP
#define WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_REGBACKUP_MANAGER_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::registry_subsystem {

/**
 * @brief Stub interface for RegBackup_Manager.
 */
class RegBackup_Manager final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::registry_subsystem

#endif
