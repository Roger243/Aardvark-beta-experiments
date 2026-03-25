#ifndef WIN_AI_AGENT_ADVANCED_CREDENTIAL_PROVIDER_BRIDGE_HPP
#define WIN_AI_AGENT_ADVANCED_CREDENTIAL_PROVIDER_BRIDGE_HPP

#include "WinAIAgent/Network/SecureCommunicator.hpp"

#include <expected>
#include <shared_mutex>
#include <string>

namespace win_ai_agent::advanced {

class CredentialProviderBridge final {
 public:
  explicit CredentialProviderBridge(const win_ai_agent::network::SecureCommunicator& communicator);
  std::expected<void, std::string> LogFailedAttempt(const std::wstring& username_hint) const;

 private:
  const win_ai_agent::network::SecureCommunicator& communicator_;
  mutable std::shared_mutex mutex_;
};

}  // namespace win_ai_agent::advanced

#endif
