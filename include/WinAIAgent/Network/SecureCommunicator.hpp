#ifndef WIN_AI_AGENT_NETWORK_SECURE_COMMUNICATOR_HPP
#define WIN_AI_AGENT_NETWORK_SECURE_COMMUNICATOR_HPP

#include "WinAIAgent/Network/NetworkManager.hpp"

#include <expected>
#include <string>

namespace win_ai_agent::network {

class SecureCommunicator final {
 public:
  explicit SecureCommunicator(const NetworkManager& network_manager);

  std::expected<void, std::string> SendTelemetryJson(const std::string& endpoint,
                                                     const nlohmann::json& payload) const;

 private:
  const NetworkManager& network_manager_;
};

}  // namespace win_ai_agent::network

#endif  // WIN_AI_AGENT_NETWORK_SECURE_COMMUNICATOR_HPP
