#include "Feedback_Loop_UI/FeedbackWeightModel.hpp"

namespace win_ai_agent_nexus_pro::feedback_loop_ui {

std::expected<void, std::string> FeedbackWeightModel::Initialize() {
  return {};
}

std::expected<std::any, std::string> FeedbackWeightModel::Execute(const std::any& input) {
  return input;
}

}  // namespace win_ai_agent_nexus_pro::feedback_loop_ui
