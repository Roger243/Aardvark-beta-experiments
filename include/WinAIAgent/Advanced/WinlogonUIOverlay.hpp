#ifndef WIN_AI_AGENT_ADVANCED_WINLOGON_UI_OVERLAY_HPP
#define WIN_AI_AGENT_ADVANCED_WINLOGON_UI_OVERLAY_HPP

#include <expected>
#include <shared_mutex>
#include <string>

namespace win_ai_agent::advanced {

class WinlogonUIOverlay final {
 public:
  std::expected<void, std::string> ShowPreLogonStatus(const std::wstring& message);

 private:
  mutable std::shared_mutex mutex_;
};

}  // namespace win_ai_agent::advanced

#endif
