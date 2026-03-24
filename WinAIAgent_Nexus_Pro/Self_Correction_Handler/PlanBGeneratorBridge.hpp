#ifndef WIN_AI_AGENT_NEXUS_PRO_SELF_CORRECTION_HANDLER_PLANBGENERATORBRIDGE_HPP
#define WIN_AI_AGENT_NEXUS_PRO_SELF_CORRECTION_HANDLER_PLANBGENERATORBRIDGE_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::self_correction_handler {

/**
 * @brief PlanBGeneratorBridge module for WinAIAgent_Nexus_Pro.
 */
class PlanBGeneratorBridge final {
 public:
  std::expected<void, std::string> Initialize();
  std::expected<std::any, std::string> Execute(const std::any& input);
};

}  // namespace win_ai_agent_nexus_pro::self_correction_handler

#endif
