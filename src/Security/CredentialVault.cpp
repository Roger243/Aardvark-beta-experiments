#include "WinAIAgent/Security/CredentialVault.hpp"

#include <windows.h>
#include <wincred.h>

namespace win_ai_agent::security {

bool CredentialVault::WriteSecret(const std::wstring& target, const std::wstring& secret) const {
  CREDENTIALW cred{};
  cred.Type = CRED_TYPE_GENERIC;
  cred.TargetName = const_cast<LPWSTR>(target.c_str());
  cred.CredentialBlobSize = static_cast<DWORD>(secret.size() * sizeof(wchar_t));
  cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(secret.data()));
  cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
  cred.UserName = const_cast<LPWSTR>(L"win-ai-agent");
  return CredWriteW(&cred, 0) == TRUE;
}

std::optional<std::wstring> CredentialVault::ReadSecret(const std::wstring& target) const {
  PCREDENTIALW credential = nullptr;
  if (!CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &credential)) {
    return std::nullopt;
  }

  std::wstring value(reinterpret_cast<wchar_t*>(credential->CredentialBlob),
                     credential->CredentialBlobSize / sizeof(wchar_t));
  CredFree(credential);
  return value;
}

}  // namespace win_ai_agent::security
