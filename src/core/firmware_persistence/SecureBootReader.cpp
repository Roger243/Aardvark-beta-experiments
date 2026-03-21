#include "src/core/firmware_persistence/SecureBootReader.hpp"

#include "src/core/integrity/UefiVariableReader.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<SecureBootState, WinErrorStruct> SecureBootReader::ReadState() const {
  UefiVariableReader reader;
  const auto secure = reader.ReadStringVariable(L"SecureBoot", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}");
  if (!secure) return std::unexpected(secure.error());
  const auto setup = reader.ReadStringVariable(L"SetupMode", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}");
  if (!setup) return std::unexpected(setup.error());
  return SecureBootState{*secure, *setup};
}

}  // namespace win_ai_agent::nexus_pro
