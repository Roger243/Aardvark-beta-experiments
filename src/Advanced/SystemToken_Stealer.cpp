#include "WinAIAgent/Advanced/SystemTokenStealer.hpp"

#include <mutex>

namespace win_ai_agent::advanced {

std::expected<void, std::string> SystemTokenStealer::ValidateElevationBoundary() {
  std::unique_lock lock(mutex_);

  // Refusal-by-design: token theft / impersonation escalation is prohibited.
  // Keep this as an explicit policy guardrail in code.
  return std::unexpected("SYSTEM token duplication is prohibited");
}

}  // namespace win_ai_agent::advanced
