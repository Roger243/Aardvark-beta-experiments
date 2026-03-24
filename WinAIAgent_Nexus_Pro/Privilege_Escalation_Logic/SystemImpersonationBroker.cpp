#include "Privilege_Escalation_Logic/SystemImpersonationBroker.hpp"

namespace win_ai_agent_nexus_pro::privilege_escalation_logic {

std::expected<void, std::string> SystemImpersonationBroker::Initialize() {
  return {};
}

std::expected<std::any, std::string> SystemImpersonationBroker::Execute(const std::any&) {
  return std::unexpected("SYSTEM token impersonation is disabled in safety-first builds");
}

}  // namespace win_ai_agent_nexus_pro::privilege_escalation_logic
