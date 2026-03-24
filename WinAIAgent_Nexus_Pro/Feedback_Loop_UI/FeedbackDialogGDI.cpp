#include "Feedback_Loop_UI/FeedbackDialogGDI.hpp"

#include <windows.h>

namespace win_ai_agent_nexus_pro::feedback_loop_ui {

std::expected<void, std::string> FeedbackDialogGDI::Initialize() {
  return {};
}

std::expected<std::any, std::string> FeedbackDialogGDI::Execute(const std::any&) {
  const int rc = MessageBoxW(nullptr, L"Did my last action help?", L"WinAIAgent Feedback",
                             MB_ICONQUESTION | MB_YESNO);
  if (rc == IDYES) {
    return std::any(true);
  }
  if (rc == IDNO) {
    return std::any(false);
  }
  return std::unexpected("Feedback dialog was dismissed unexpectedly");
}

}  // namespace win_ai_agent_nexus_pro::feedback_loop_ui
