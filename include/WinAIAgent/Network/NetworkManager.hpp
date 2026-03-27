#ifndef WIN_AI_AGENT_NETWORK_NETWORK_MANAGER_HPP
#define WIN_AI_AGENT_NETWORK_NETWORK_MANAGER_HPP

#include <nlohmann/json.hpp>

#include <chrono>
#include <future>
#include <optional>
#include <string>
#include <vector>

namespace win_ai_agent::network {

struct HttpRequest final {
  std::string url;
  std::string method{"GET"};
  std::vector<std::string> headers;
  nlohmann::json payload;
  std::chrono::milliseconds timeout{30000};
  int max_retries{4};
};

struct HttpResponse final {
  long status_code{};
  std::string body;
  std::string error;
  bool success{};
};

class NetworkManager final {
 public:
  NetworkManager();
  ~NetworkManager();

  NetworkManager(const NetworkManager&) = delete;
  NetworkManager& operator=(const NetworkManager&) = delete;

  std::future<HttpResponse> SendAsync(const HttpRequest& request) const;
  bool StoreApiKeySecurely(const std::wstring& target, const std::wstring& api_key) const;
  std::optional<std::wstring> LoadApiKeySecurely(const std::wstring& target) const;

 private:
  HttpResponse SendWithRetry(const HttpRequest& request) const;
};

}  // namespace win_ai_agent::network

#endif  // WIN_AI_AGENT_NETWORK_NETWORK_MANAGER_HPP
