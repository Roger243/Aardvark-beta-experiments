#include "WinAIAgent/Network/NetworkManager.hpp"

#include <windows.h>
#include <wincred.h>
#include <winhttp.h>

#include <curl/curl.h>

#include <cmath>
#include <future>
#include <random>
#include <thread>

namespace win_ai_agent::network {
namespace {

size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
  const auto total = size * nmemb;
  auto* out = static_cast<std::string*>(userdata);
  out->append(ptr, total);
  return total;
}

std::optional<std::string> DiscoverSystemProxy(const std::string& url) {
  WINHTTP_AUTOPROXY_OPTIONS options{};
  options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
  options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
  options.fAutoLogonIfChallenged = TRUE;

  WINHTTP_PROXY_INFO info{};
  if (!WinHttpGetProxyForUrl(nullptr, std::wstring(url.begin(), url.end()).c_str(), &options, &info)) {
    return std::nullopt;
  }

  std::optional<std::string> proxy;
  if (info.lpszProxy != nullptr) {
    std::wstring wproxy(info.lpszProxy);
    proxy = std::string(wproxy.begin(), wproxy.end());
  }

  if (info.lpszProxy != nullptr) {
    GlobalFree(info.lpszProxy);
  }
  if (info.lpszProxyBypass != nullptr) {
    GlobalFree(info.lpszProxyBypass);
  }

  return proxy;
}

std::chrono::milliseconds ComputeBackoff(int attempt) {
  constexpr int base = 250;
  const auto exponential = static_cast<int>(std::pow(2.0, attempt));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> jitter(0, 300);
  return std::chrono::milliseconds(base * exponential + jitter(gen));
}

}  // namespace

NetworkManager::NetworkManager() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

NetworkManager::~NetworkManager() {
  curl_global_cleanup();
}

std::future<HttpResponse> NetworkManager::SendAsync(const HttpRequest& request) const {
  return std::async(std::launch::async, [this, request] { return SendWithRetry(request); });
}

HttpResponse NetworkManager::SendWithRetry(const HttpRequest& request) const {
  HttpResponse final_response{};

  for (int attempt = 0; attempt <= request.max_retries; ++attempt) {
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
      return {.status_code = 0, .body = {}, .error = "curl_easy_init failed", .success = false};
    }

    std::string response_body;
    curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request.timeout.count());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    auto proxy = DiscoverSystemProxy(request.url);
    if (proxy.has_value() && !proxy->empty()) {
      curl_easy_setopt(curl, CURLOPT_PROXY, proxy->c_str());
    }

    struct curl_slist* header_list = nullptr;
    for (const auto& header : request.headers) {
      header_list = curl_slist_append(header_list, header.c_str());
    }
    header_list = curl_slist_append(header_list, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    std::string payload;
    if (request.method == "POST" || request.method == "PUT") {
      payload = request.payload.dump();
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
      if (request.method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
      } else {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
      }
    } else if (request.method == "DELETE") {
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    const CURLcode rc = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (header_list != nullptr) {
      curl_slist_free_all(header_list);
    }
    curl_easy_cleanup(curl);

    final_response.status_code = status;
    final_response.body = std::move(response_body);

    if (rc == CURLE_OK && status >= 200 && status < 300) {
      final_response.success = true;
      return final_response;
    }

    final_response.success = false;
    final_response.error = curl_easy_strerror(rc);

    if (attempt < request.max_retries) {
      std::this_thread::sleep_for(ComputeBackoff(attempt));
      continue;
    }
  }

  return final_response;
}

bool NetworkManager::StoreApiKeySecurely(const std::wstring& target, const std::wstring& api_key) const {
  CREDENTIALW cred{};
  cred.Type = CRED_TYPE_GENERIC;
  cred.TargetName = const_cast<LPWSTR>(target.c_str());
  cred.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(api_key.data()));
  cred.CredentialBlobSize = static_cast<DWORD>(api_key.size() * sizeof(wchar_t));
  cred.Persist = CRED_PERSIST_ENTERPRISE;
  cred.UserName = const_cast<LPWSTR>(L"openai");
  return CredWriteW(&cred, 0) == TRUE;
}

std::optional<std::wstring> NetworkManager::LoadApiKeySecurely(const std::wstring& target) const {
  PCREDENTIALW credential = nullptr;
  if (!CredReadW(target.c_str(), CRED_TYPE_GENERIC, 0, &credential)) {
    return std::nullopt;
  }

  std::wstring key(reinterpret_cast<wchar_t*>(credential->CredentialBlob),
                   credential->CredentialBlobSize / sizeof(wchar_t));
  CredFree(credential);
  return key;
}

}  // namespace win_ai_agent::network
