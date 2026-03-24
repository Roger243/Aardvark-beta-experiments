#ifndef WIN_AI_AGENT_NEXUS_PRO_UI_AUTOMATION_SCANNER_VISIBLETEXTSAMPLER_HPP
#define WIN_AI_AGENT_NEXUS_PRO_UI_AUTOMATION_SCANNER_VISIBLETEXTSAMPLER_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::ui_automation_scanner {

class VisibleTextSampler final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input) const;
};

}  // namespace win_ai_agent_nexus_pro::ui_automation_scanner

#endif
