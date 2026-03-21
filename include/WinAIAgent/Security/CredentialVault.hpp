#ifndef WIN_AI_AGENT_SECURITY_CREDENTIAL_VAULT_HPP
#define WIN_AI_AGENT_SECURITY_CREDENTIAL_VAULT_HPP

#include <optional>
#include <string>

namespace win_ai_agent::security {

class CredentialVault final {
 public:
  bool WriteSecret(const std::wstring& target, const std::wstring& secret) const;
  std::optional<std::wstring> ReadSecret(const std::wstring& target) const;
};

}  // namespace win_ai_agent::security

#endif  // WIN_AI_AGENT_SECURITY_CREDENTIAL_VAULT_HPP
