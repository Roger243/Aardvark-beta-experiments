#include "WinAIAgent/Advanced/CredentialProviderBridge.hpp"

#include <mutex>

namespace win_ai_agent::advanced {

CredentialProviderBridge::CredentialProviderBridge(
    const win_ai_agent::network::SecureCommunicator& communicator)
    : communicator_(communicator) {}

std::expected<void, std::string> CredentialProviderBridge::LogFailedAttempt(
    const std::wstring& username_hint) const {
  std::shared_lock lock(mutex_);

  nlohmann::json payload{
      {"event", "failed_logon_attempt"},
      {"username_hint", std::string(username_hint.begin(), username_hint.end())},
      {"source", "credential_provider_bridge"},
  };

  return communicator_.SendTelemetryJson("https://example.invalid/logon-events", payload);
}

}  // namespace win_ai_agent::advanced
