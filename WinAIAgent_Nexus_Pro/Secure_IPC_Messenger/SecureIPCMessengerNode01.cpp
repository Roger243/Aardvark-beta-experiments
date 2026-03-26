#include "Secure_IPC_Messenger/SecureIPCMessengerNode01.hpp"

#include <windows.h>

namespace win_ai_agent_nexus_pro::secure_ipc_messenger {

std::expected<nlohmann::json, std::string> SecureIPCMessengerNode01::Execute(
    const nlohmann::json& input) {
  HANDLE pipe = CreateNamedPipeW(
      L"\\.\\pipe\\WinAIAgentSecureIPC", PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, 4096, 4096, 0, nullptr);

  if (pipe == INVALID_HANDLE_VALUE) {
    return std::unexpected("CreateNamedPipeW failed");
  }

  // AES-GCM key provider probe (actual key exchange defined in follow-up module iterations).
  BCRYPT_ALG_HANDLE alg = nullptr;
  if (BCryptOpenAlgorithmProvider(&alg, BCRYPT_AES_ALGORITHM, nullptr, 0) < 0) {
    CloseHandle(pipe);
    return std::unexpected("BCrypt AES provider unavailable");
  }
  BCryptCloseAlgorithmProvider(alg, 0);
  CloseHandle(pipe);

  nlohmann::json out = input;
  out["module"] = "SecureIPCMessengerNode01";
  out["healthy"] = healthy_.load();
  out["transport"] = "named_pipe";
  out["crypto"] = "aes_gcm_provider_ok";
  return out;
}

}  // namespace win_ai_agent_nexus_pro::secure_ipc_messenger
