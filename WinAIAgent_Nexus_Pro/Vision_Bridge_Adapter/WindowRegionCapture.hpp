#ifndef WIN_AI_AGENT_NEXUS_PRO_VISION_BRIDGE_ADAPTER_WINDOWREGIONCAPTURE_HPP
#define WIN_AI_AGENT_NEXUS_PRO_VISION_BRIDGE_ADAPTER_WINDOWREGIONCAPTURE_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::vision_bridge_adapter {

class WindowRegionCapture final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input) const;
};

}  // namespace win_ai_agent_nexus_pro::vision_bridge_adapter

#endif
