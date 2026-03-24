#ifndef WIN_AI_AGENT_NEXUS_PRO_FEEDBACK_LOOP_UI_FEEDBACKWEIGHTMODEL_HPP
#define WIN_AI_AGENT_NEXUS_PRO_FEEDBACK_LOOP_UI_FEEDBACKWEIGHTMODEL_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::feedback_loop_ui {

/**
 * @brief FeedbackWeightModel module for WinAIAgent_Nexus_Pro.
 */
class FeedbackWeightModel final {
 public:
  std::expected<void, std::string> Initialize();
  std::expected<std::any, std::string> Execute(const std::any& input);
};

}  // namespace win_ai_agent_nexus_pro::feedback_loop_ui

#endif
