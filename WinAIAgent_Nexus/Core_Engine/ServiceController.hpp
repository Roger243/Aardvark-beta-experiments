#ifndef WIN_AI_AGENT_NEXUS_CORE_ENGINE_SERVICECONTROLLER_HPP
#define WIN_AI_AGENT_NEXUS_CORE_ENGINE_SERVICECONTROLLER_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::core_engine {

/**
 * @brief Stub interface for ServiceController.
 */
class ServiceController final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::core_engine

#endif
