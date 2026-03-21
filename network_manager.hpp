#pragma once

#include <windows.h>
#include <wininet.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#pragma comment(lib, "wininet.lib")

namespace agent {

class WinInetHandle final {
 public:
  explicit WinInetHandle(HINTERNET handle = nullptr) noexcept : handle_(handle) {}

  WinInetHandle(const WinInetHandle&) = delete;
  WinInetHandle& operator=(const WinInetHandle&) = delete;

  WinInetHandle(WinInetHandle&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }

  WinInetHandle& operator=(WinInetHandle&& other) noexcept {
    if (this != &other) {
      reset();
      handle_ = other.handle_;
      other.handle_ = nullptr;
    }
    return *this;
  }

  ~WinInetHandle() { reset(); }

  [[nodiscard]] HINTERNET get() const noexcept { return handle_; }

  [[nodiscard]] explicit operator bool() const noexcept { return handle_ != nullptr; }

  void reset(HINTERNET next = nullptr) noexcept {
    if (handle_ != nullptr) {
      InternetCloseHandle(handle_);
    }
    handle_ = next;
  }

 private:
  HINTERNET handle_;
};

struct HttpResponse final {
  DWORD status_code{};
  std::string body;
};

class NetworkManager final {
 public:
  explicit NetworkManager(std::wstring user_agent = L"AardvarkAgent/1.0")
      : user_agent_(std::move(user_agent)),
        internet_(InternetOpenW(user_agent_.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, nullptr,
                                nullptr, 0)) {
    if (!internet_) {
      throw std::runtime_error("InternetOpenW failed");
    }
  }

  [[nodiscard]] HttpResponse HttpGet(const std::wstring& host,
                                     const std::wstring& path,
                                     INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT,
                                     bool secure = true,
                                     std::chrono::milliseconds timeout =
                                         std::chrono::milliseconds{10000}) const {
    return PerformRequest(L"GET", host, path, "", port, secure, timeout);
  }

  [[nodiscard]] HttpResponse HttpPost(const std::wstring& host,
                                      const std::wstring& path,
                                      std::string_view body,
                                      INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT,
                                      bool secure = true,
                                      std::chrono::milliseconds timeout =
                                          std::chrono::milliseconds{10000}) const {
    return PerformRequest(L"POST", host, path, body, port, secure, timeout);
  }

 private:
  [[nodiscard]] HttpResponse PerformRequest(const std::wstring& method,
                                            const std::wstring& host,
                                            const std::wstring& path,
                                            std::string_view body,
                                            INTERNET_PORT port,
                                            bool secure,
                                            std::chrono::milliseconds timeout) const {
    const DWORD timeout_ms = static_cast<DWORD>(timeout.count());
    InternetSetOptionW(internet_.get(), INTERNET_OPTION_CONNECT_TIMEOUT,
                       const_cast<DWORD*>(&timeout_ms), sizeof(timeout_ms));
    InternetSetOptionW(internet_.get(), INTERNET_OPTION_SEND_TIMEOUT,
                       const_cast<DWORD*>(&timeout_ms), sizeof(timeout_ms));
    InternetSetOptionW(internet_.get(), INTERNET_OPTION_RECEIVE_TIMEOUT,
                       const_cast<DWORD*>(&timeout_ms), sizeof(timeout_ms));

    WinInetHandle connection(
        InternetConnectW(internet_.get(), host.c_str(), port, nullptr, nullptr,
                         INTERNET_SERVICE_HTTP, 0, 0));
    if (!connection) {
      throw std::runtime_error("InternetConnectW failed");
    }

    constexpr std::wstring_view accept_types[] = {L"application/json", L"*/*"};
    const wchar_t* accept_ptrs[] = {accept_types[0].data(), accept_types[1].data(), nullptr};

    DWORD flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD;
    if (secure) {
      flags |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;
    }

    WinInetHandle request( HttpOpenRequestW(connection.get(), method.c_str(), path.c_str(), nullptr,
                                            nullptr, accept_ptrs, flags, 0));
    if (!request) {
      throw std::runtime_error("HttpOpenRequestW failed");
    }

    std::string headers = "Content-Type: application/json\r\n";
    BOOL sent = HttpSendRequestA(request.get(), headers.c_str(), static_cast<DWORD>(headers.size()),
                                 body.empty() ? nullptr : const_cast<char*>(body.data()),
                                 static_cast<DWORD>(body.size()));
    if (!sent) {
      throw std::runtime_error("HttpSendRequestA failed");
    }

    DWORD status_code = 0;
    DWORD status_size = sizeof(status_code);
    if (!HttpQueryInfoW(request.get(), HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                        &status_code, &status_size, nullptr)) {
      throw std::runtime_error("HttpQueryInfoW status failed");
    }

    std::string response_body;
    std::vector<char> chunk(4096);
    DWORD read = 0;
    while (InternetReadFile(request.get(), chunk.data(), static_cast<DWORD>(chunk.size()), &read) &&
           read != 0) {
      response_body.append(chunk.data(), chunk.data() + read);
    }

    return HttpResponse{status_code, response_body};
  }

  std::wstring user_agent_;
  WinInetHandle internet_;
};

}  // namespace agent
