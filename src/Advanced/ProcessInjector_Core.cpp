#include "WinAIAgent/Advanced/ProcessInjectorCore.hpp"

#include <mutex>

namespace win_ai_agent::advanced {

std::expected<void, std::string> ProcessInjectorCore::MonitorSessionProcessesSafely() {
  std::unique_lock lock(mutex_);

  // Refusal-by-design: this module intentionally does NOT perform code injection.
  // It exists as a safe compatibility layer where process lifecycle events can be
  // observed through legitimate telemetry APIs.
  return std::unexpected("Process injection is disabled for security and policy compliance");
}

}  // namespace win_ai_agent::advanced
