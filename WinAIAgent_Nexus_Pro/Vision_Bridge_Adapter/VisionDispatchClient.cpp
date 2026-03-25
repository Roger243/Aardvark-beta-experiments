#include "Vision_Bridge_Adapter/VisionDispatchClient.hpp"

namespace win_ai_agent_nexus_pro::vision_bridge_adapter {

std::expected<std::any, std::string> VisionDispatchClient::Execute(const std::any& input) const {
  return input;
}

}  // namespace win_ai_agent_nexus_pro::vision_bridge_adapter
