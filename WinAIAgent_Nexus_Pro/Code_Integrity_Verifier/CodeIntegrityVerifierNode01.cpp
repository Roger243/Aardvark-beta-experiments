#include "Code_Integrity_Verifier/CodeIntegrityVerifierNode01.hpp"

#include <bcrypt.h>

#pragma comment(lib, "bcrypt.lib")

namespace win_ai_agent_nexus_pro::code_integrity_verifier {

std::expected<nlohmann::json, std::string> CodeIntegrityVerifierNode01::Execute(
    const nlohmann::json& input) {
  BCRYPT_ALG_HANDLE alg = nullptr;
  NTSTATUS status = BCryptOpenAlgorithmProvider(&alg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
  if (status < 0) {
    return std::unexpected("BCryptOpenAlgorithmProvider failed");
  }

  BCryptCloseAlgorithmProvider(alg, 0);

  nlohmann::json out = input;
  out["module"] = "CodeIntegrityVerifierNode01";
  out["healthy"] = healthy_.load();
  out["integrity_check"] = "sha256_provider_ok";
  return out;
}

}  // namespace win_ai_agent_nexus_pro::code_integrity_verifier
