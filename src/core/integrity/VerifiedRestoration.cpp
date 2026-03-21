#include "src/core/integrity/VerifiedRestoration.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> VerifiedRestoration::Restore(const RestorationRequest& request,
                                                                 const std::string& required_guid) const {
  if (request.operator_confirmation_guid != required_guid) {
    return std::unexpected(WinErrorStruct{13, "Manual GUID confirmation mismatch"});
  }
  return {};
}

}  // namespace win_ai_agent::nexus_pro
