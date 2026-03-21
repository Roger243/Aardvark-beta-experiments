#ifndef WIN_AI_AGENT_NEXUS_CORE_ENGINE_INTERNALMESSAGEBUS_HPP
#define WIN_AI_AGENT_NEXUS_CORE_ENGINE_INTERNALMESSAGEBUS_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::core_engine {

/**
 * @brief Stub interface for InternalMessageBus.
 */
class InternalMessageBus final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::core_engine

#endif
