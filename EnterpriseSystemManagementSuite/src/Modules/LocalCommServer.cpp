#include "EnterpriseSuite/Modules/LocalCommServerImpl.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

#include <windows.h>

namespace EnterpriseSuite::Modules {
namespace {

constexpr DWORD kBufferSize = 64 * 1024;

bool ReadExactly(HANDLE pipe, std::uint8_t* buffer, DWORD expected_bytes) {
    DWORD total_read = 0;
    while (total_read < expected_bytes) {
        DWORD chunk = 0;
        if (!ReadFile(pipe, buffer + total_read, expected_bytes - total_read, &chunk, nullptr)) {
            return false;
        }
        if (chunk == 0) {
            return false;
        }
        total_read += chunk;
    }
    return true;
}

bool WriteExactly(HANDLE pipe, const std::uint8_t* buffer, DWORD expected_bytes) {
    DWORD total_written = 0;
    while (total_written < expected_bytes) {
        DWORD chunk = 0;
        if (!WriteFile(pipe, buffer + total_written, expected_bytes - total_written, &chunk, nullptr)) {
            return false;
        }
        if (chunk == 0) {
            return false;
        }
        total_written += chunk;
    }
    return true;
}

}  // namespace

NamedPipeLocalCommServer::NamedPipeLocalCommServer() = default;

NamedPipeLocalCommServer::~NamedPipeLocalCommServer() {
    const auto _ = Stop();
    (void)_;
}

Common::OperationResult NamedPipeLocalCommServer::Start(const std::string& endpoint_name) {
    const std::scoped_lock lock(mutex_);

    if (listening_.load()) {
        return {.success = false, .code = "ALREADY_RUNNING", .message = "Local communication server is already running."};
    }

    if (endpoint_name.empty()) {
        return {.success = false, .code = "INVALID_ENDPOINT", .message = "Endpoint name must not be empty."};
    }

    endpoint_name_ = endpoint_name;
    listening_.store(true);
    server_thread_ = std::thread(&NamedPipeLocalCommServer::ServerLoop, this);

    return {.success = true, .code = "OK", .message = "Local communication server started."};
}

Common::OperationResult NamedPipeLocalCommServer::Stop() {
    {
        const std::scoped_lock lock(mutex_);
        if (!listening_.load()) {
            return {.success = true, .code = "OK", .message = "Local communication server is already stopped."};
        }
        listening_.store(false);
    }

    // Open and immediately close to unblock ConnectNamedPipe in server thread.
    const auto pipe_name = BuildPipeName(endpoint_name_);
    HANDLE wake = CreateFileA(pipe_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (wake != INVALID_HANDLE_VALUE) {
        CloseHandle(wake);
    }

    if (server_thread_.joinable()) {
        server_thread_.join();
    }

    return {.success = true, .code = "OK", .message = "Local communication server stopped."};
}

bool NamedPipeLocalCommServer::IsListening() const {
    return listening_.load();
}

void NamedPipeLocalCommServer::SetMessageHandler(MessageHandler handler) {
    const std::scoped_lock lock(mutex_);
    handler_ = std::move(handler);
}

void NamedPipeLocalCommServer::ServerLoop() {
    const auto pipe_name = BuildPipeName(endpoint_name_);

    while (listening_.load()) {
        HANDLE pipe = CreateNamedPipeA(
            pipe_name.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            kBufferSize,
            kBufferSize,
            0,
            nullptr);

        if (pipe == INVALID_HANDLE_VALUE) {
            break;
        }

        const BOOL connected = ConnectNamedPipe(pipe, nullptr)
            ? TRUE
            : (GetLastError() == ERROR_PIPE_CONNECTED ? TRUE : FALSE);

        if (!connected) {
            CloseHandle(pipe);
            continue;
        }

        std::array<std::uint8_t, sizeof(std::uint32_t)> header{};
        if (!ReadExactly(pipe, header.data(), static_cast<DWORD>(header.size()))) {
            DisconnectNamedPipe(pipe);
            CloseHandle(pipe);
            continue;
        }

        std::uint32_t payload_size = 0;
        std::memcpy(&payload_size, header.data(), sizeof(payload_size));
        if (payload_size == 0 || payload_size > kBufferSize) {
            DisconnectNamedPipe(pipe);
            CloseHandle(pipe);
            continue;
        }

        std::vector<std::uint8_t> payload(payload_size);
        if (!ReadExactly(pipe, payload.data(), payload_size)) {
            DisconnectNamedPipe(pipe);
            CloseHandle(pipe);
            continue;
        }

        MessageHandler local_handler;
        {
            const std::scoped_lock lock(mutex_);
            local_handler = handler_;
        }

        std::vector<std::uint8_t> response;
        if (local_handler) {
            response = local_handler(LocalMessage{.client_id = "local_named_pipe_client", .payload = payload});
        }

        const auto response_size = static_cast<std::uint32_t>(
            std::min<std::size_t>(response.size(), static_cast<std::size_t>(kBufferSize)));

        std::array<std::uint8_t, sizeof(response_size)> response_header{};
        std::memcpy(response_header.data(), &response_size, sizeof(response_size));

        if (WriteExactly(pipe, response_header.data(), static_cast<DWORD>(response_header.size())) && response_size > 0) {
            WriteExactly(pipe, response.data(), response_size);
        }

        FlushFileBuffers(pipe);
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
    }
}

std::string NamedPipeLocalCommServer::BuildPipeName(const std::string& endpoint_name) const {
    return R"(\\.\pipe\)" + endpoint_name;
}

}  // namespace EnterpriseSuite::Modules
