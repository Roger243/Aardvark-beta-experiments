#include "WinAIAgent/Security/ScriptExecutor.hpp"

#include <windows.h>
#include <wincrypt.h>
#include <wintrust.h>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#pragma comment(lib, "wintrust.lib")

namespace fs = std::filesystem;

namespace win_ai_agent::security {
namespace {

class CircularBuffer final {
 public:
  explicit CircularBuffer(std::size_t capacity) : data_(capacity, '\0') {}

  void Append(const char* bytes, std::size_t count) {
    std::lock_guard<std::mutex> lock(mu_);
    for (std::size_t i = 0; i < count; ++i) {
      data_[head_] = bytes[i];
      head_ = (head_ + 1) % data_.size();
      if (size_ < data_.size()) {
        ++size_;
      }
    }
  }

  [[nodiscard]] std::string Dump() const {
    std::lock_guard<std::mutex> lock(mu_);
    std::string out;
    out.reserve(size_);

    const std::size_t start = (head_ + data_.size() - size_) % data_.size();
    for (std::size_t i = 0; i < size_; ++i) {
      out.push_back(data_[(start + i) % data_.size()]);
    }
    return out;
  }

 private:
  mutable std::mutex mu_;
  std::vector<char> data_;
  std::size_t head_{};
  std::size_t size_{};
};

struct PipeHandles final {
  HANDLE read_pipe{nullptr};
  HANDLE write_pipe{nullptr};

  ~PipeHandles() {
    if (read_pipe != nullptr) {
      CloseHandle(read_pipe);
    }
    if (write_pipe != nullptr) {
      CloseHandle(write_pipe);
    }
  }
};

std::wstring WriteTempScript(const std::string& script, ScriptType type) {
  const auto root = fs::temp_directory_path() / "win_ai_agent" / "scripts";
  fs::create_directories(root);

  const auto ext = (type == ScriptType::PowerShell) ? L".ps1" : L".cmd";
  const auto tick = std::to_wstring(GetTickCount64());
  const auto path = root / (L"script_" + tick + ext);

  std::ofstream out(path, std::ios::binary);
  out.write(script.data(), static_cast<std::streamsize>(script.size()));
  out.close();

  return path.wstring();
}

void ReadPipeToCircularBuffer(HANDLE read_pipe, CircularBuffer* buffer, HANDLE process_handle) {
  constexpr DWORD kChunk = 2048;
  std::vector<char> chunk(kChunk);

  while (WaitForSingleObject(process_handle, 20) == WAIT_TIMEOUT) {
    DWORD available = 0;
    if (!PeekNamedPipe(read_pipe, nullptr, 0, nullptr, &available, nullptr)) {
      break;
    }

    if (available == 0) {
      continue;
    }

    DWORD read = 0;
    if (ReadFile(read_pipe, chunk.data(), kChunk, &read, nullptr) && read > 0) {
      buffer->Append(chunk.data(), read);
    }
  }

  DWORD read = 0;
  while (ReadFile(read_pipe, chunk.data(), kChunk, &read, nullptr) && read > 0) {
    buffer->Append(chunk.data(), read);
  }
}

}  // namespace

ScriptExecutor::ScriptExecutor() = default;
ScriptExecutor::~ScriptExecutor() = default;

ScriptExecutionResult ScriptExecutor::ExecuteReceivedScript(const std::string& script,
                                                            ScriptType type,
                                                            std::chrono::milliseconds timeout,
                                                            std::size_t circular_buffer_bytes) const {
  ScriptExecutionResult result{};

  const std::wstring script_path = WriteTempScript(script, type);
  const fs::path cleanup_path(script_path);

  if (!VerifyScriptSignature(script_path)) {
    result.error = "Script signature verification failed";
    std::error_code ec;
    fs::remove(cleanup_path, ec);
    return result;
  }

  SECURITY_ATTRIBUTES sa{};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;

  PipeHandles stdout_pipe;
  if (!CreatePipe(&stdout_pipe.read_pipe, &stdout_pipe.write_pipe, &sa, 0)) {
    result.error = "CreatePipe failed";
    std::error_code ec;
    fs::remove(cleanup_path, ec);
    return result;
  }

  SetHandleInformation(stdout_pipe.read_pipe, HANDLE_FLAG_INHERIT, 0);

  STARTUPINFOW si{};
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdOutput = stdout_pipe.write_pipe;
  si.hStdError = stdout_pipe.write_pipe;

  PROCESS_INFORMATION pi{};
  std::wstring cmd_line;
  if (type == ScriptType::PowerShell) {
    cmd_line = L"powershell.exe -NoProfile -ExecutionPolicy AllSigned -File \"" + script_path + L"\"";
  } else {
    cmd_line = L"cmd.exe /c \"" + script_path + L"\"";
  }

  std::vector<wchar_t> mutable_cmd(cmd_line.begin(), cmd_line.end());
  mutable_cmd.push_back(L'\0');

  if (!CreateProcessW(nullptr, mutable_cmd.data(), nullptr, nullptr, TRUE,
                      CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    result.error = "CreateProcessW failed";
    std::error_code ec;
    fs::remove(cleanup_path, ec);
    return result;
  }

  CloseHandle(pi.hThread);
  CloseHandle(stdout_pipe.write_pipe);
  stdout_pipe.write_pipe = nullptr;

  CircularBuffer circular(circular_buffer_bytes);
  std::thread pump(ReadPipeToCircularBuffer, stdout_pipe.read_pipe, &circular, pi.hProcess);

  const DWORD wait_rc = WaitForSingleObject(pi.hProcess, static_cast<DWORD>(timeout.count()));
  if (wait_rc == WAIT_TIMEOUT) {
    TerminateProcess(pi.hProcess, 124);
    result.timed_out = true;
  }

  WaitForSingleObject(pi.hProcess, 3000);
  if (pump.joinable()) {
    pump.join();
  }

  GetExitCodeProcess(pi.hProcess, &result.exit_code);
  CloseHandle(pi.hProcess);

  result.success = (!result.timed_out && result.exit_code == 0);
  result.circular_log = circular.Dump();

  std::error_code ec;
  fs::remove(cleanup_path, ec);
  return result;
}

bool ScriptExecutor::VerifyScriptSignature(const std::wstring& path) const {
  GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;

  WINTRUST_FILE_INFO file_info{};
  file_info.cbStruct = sizeof(file_info);
  file_info.pcwszFilePath = path.c_str();

  WINTRUST_DATA trust_data{};
  trust_data.cbStruct = sizeof(trust_data);
  trust_data.dwUIChoice = WTD_UI_NONE;
  trust_data.fdwRevocationChecks = WTD_REVOKE_NONE;
  trust_data.dwUnionChoice = WTD_CHOICE_FILE;
  trust_data.pFile = &file_info;
  trust_data.dwStateAction = WTD_STATEACTION_VERIFY;
  trust_data.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

  const LONG verify = WinVerifyTrust(nullptr, &action, &trust_data);

  trust_data.dwStateAction = WTD_STATEACTION_CLOSE;
  (void)WinVerifyTrust(nullptr, &action, &trust_data);

  return verify == ERROR_SUCCESS;
}

}  // namespace win_ai_agent::security
