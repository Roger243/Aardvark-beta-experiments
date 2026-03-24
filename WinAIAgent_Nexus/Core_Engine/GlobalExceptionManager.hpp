#ifndef WIN_AI_AGENT_NEXUS_CORE_ENGINE_GLOBALEXCEPTIONMANAGER_HPP
#define WIN_AI_AGENT_NEXUS_CORE_ENGINE_GLOBALEXCEPTIONMANAGER_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::core_engine {

/**
 * @brief Stub interface for GlobalExceptionManager.
 */
class GlobalExceptionManager final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::core_engine

#endif
