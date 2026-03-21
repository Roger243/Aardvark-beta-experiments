#ifndef WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_USERPREFERENCES_ENGINE_HPP
#define WIN_AI_AGENT_NEXUS_REGISTRY_SUBSYSTEM_USERPREFERENCES_ENGINE_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::registry_subsystem {

/**
 * @brief Stub interface for UserPreferences_Engine.
 */
class UserPreferences_Engine final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::registry_subsystem

#endif
