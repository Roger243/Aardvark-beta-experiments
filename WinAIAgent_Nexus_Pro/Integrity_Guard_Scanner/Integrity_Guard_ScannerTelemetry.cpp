#include "Integrity_Guard_Scanner/Integrity_Guard_ScannerTelemetry.hpp"

namespace win_ai_agent_nexus_pro::integrity_guard_scanner {

std::expected<std::any, std::string> Integrity_Guard_ScannerTelemetry::Execute(const std::any& input) {
  std::scoped_lock lock(mutex_);
  return input;
}

}  // namespace win_ai_agent_nexus_pro::integrity_guard_scanner
