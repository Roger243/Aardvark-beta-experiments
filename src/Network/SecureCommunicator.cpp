#include "WinAIAgent/Network/SecureCommunicator.hpp"

namespace win_ai_agent::network {

SecureCommunicator::SecureCommunicator(const NetworkManager& network_manager)
    : network_manager_(network_manager) {}

std::expected<void, std::string> SecureCommunicator::SendTelemetryJson(
    const std::string& endpoint, const nlohmann::json& payload) const {
  HttpRequest request{};
  request.url = endpoint;
  request.method = "POST";
  request.payload = payload;
  request.headers = {"Accept: application/json"};

  auto future = network_manager_.SendAsync(request);
  const HttpResponse response = future.get();
  if (!response.success) {
    return std::unexpected(response.error.empty() ? "secure transport failed" : response.error);
  }
  return {};
}

}  // namespace win_ai_agent::network
