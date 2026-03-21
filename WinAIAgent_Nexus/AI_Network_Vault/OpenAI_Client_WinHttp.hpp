#ifndef WIN_AI_AGENT_NEXUS_AI_NETWORK_VAULT_OPENAI_CLIENT_WINHTTP_HPP
#define WIN_AI_AGENT_NEXUS_AI_NETWORK_VAULT_OPENAI_CLIENT_WINHTTP_HPP

#include <expected>
#include <string>

namespace win_ai_agent_nexus::ai_network_vault {

/**
 * @brief Stub interface for OpenAI_Client_WinHttp.
 */
class OpenAI_Client_WinHttp final {
 public:
  std::expected<void, std::string> Initialize();
};

}  // namespace win_ai_agent_nexus::ai_network_vault

#endif
