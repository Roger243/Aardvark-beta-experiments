#ifndef WIN_AI_AGENT_NEXUS_KERNEL_BRIDGE_HANDLESCANNER_HPP
#define WIN_AI_AGENT_NEXUS_KERNEL_BRIDGE_HANDLESCANNER_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::kernel_bridge {

/**
 * @brief Stub interface for HandleScanner.
 */
class HandleScanner final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::kernel_bridge

#endif
