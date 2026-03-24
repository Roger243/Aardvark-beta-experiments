#ifndef WIN_AI_AGENT_NEXUS_KERNEL_BRIDGE_PROCESSMONITOR_HPP
#define WIN_AI_AGENT_NEXUS_KERNEL_BRIDGE_PROCESSMONITOR_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::kernel_bridge {

/**
 * @brief Stub interface for ProcessMonitor.
 */
class ProcessMonitor final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::kernel_bridge

#endif
