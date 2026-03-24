#ifndef WIN_AI_AGENT_NEXUS_PRO_AI_SELF_CORRECTION_BRIDGE_PLANREFINEMENTBRIDGE_HPP
#define WIN_AI_AGENT_NEXUS_PRO_AI_SELF_CORRECTION_BRIDGE_PLANREFINEMENTBRIDGE_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace win_ai_agent_nexus_pro::ai_self_correction_bridge {

struct SystemEvent final {
  std::string domain;
  nlohmann::json payload;
};

using TaskNode = std::variant<std::string, SystemEvent>;

class PlanRefinementBridge final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const TaskNode& node) const;
};

}  // namespace win_ai_agent_nexus_pro::ai_self_correction_bridge

#endif
