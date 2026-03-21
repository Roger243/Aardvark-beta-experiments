#ifndef WIN_AI_AGENT_ADVANCED_SYSTEM_TOKEN_STEALER_HPP
#define WIN_AI_AGENT_ADVANCED_SYSTEM_TOKEN_STEALER_HPP

#include <expected>
#include <shared_mutex>
#include <string>

namespace win_ai_agent::advanced {

class SystemTokenStealer final {
 public:
  std::expected<void, std::string> ValidateElevationBoundary();

 private:
  mutable std::shared_mutex mutex_;
};

}  // namespace win_ai_agent::advanced

#endif
