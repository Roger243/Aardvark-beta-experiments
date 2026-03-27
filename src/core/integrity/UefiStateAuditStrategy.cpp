#include "src/core/integrity/UefiStateAuditStrategy.hpp"

namespace win_ai_agent::nexus_pro {

UefiStateAuditStrategy::UefiStateAuditStrategy(std::string baseline_path)
    : baseline_path_(std::move(baseline_path)) {}

std::expected<nlohmann::json, WinErrorStruct> UefiStateAuditStrategy::RunAudit() {
  auto baseline = baseline_.Load(baseline_path_);
  if (!baseline) {
    return std::unexpected(baseline.error());
  }

  const auto secure_boot = reader_.ReadStringVariable(L"SecureBoot", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}");
  if (!secure_boot) return std::unexpected(secure_boot.error());
  const auto setup_mode = reader_.ReadStringVariable(L"SetupMode", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}");
  if (!setup_mode) return std::unexpected(setup_mode.error());
  const auto audit_mode = reader_.ReadStringVariable(L"AuditMode", L"{8be4df61-93ca-11d2-aa0d-00e098032b8c}");
  if (!audit_mode) return std::unexpected(audit_mode.error());

  nlohmann::json report;
  report["module"] = "UEFI_State_Scanner";
  report["current"] = serializer_.BuildState(*secure_boot, *setup_mode, *audit_mode);
  report["baseline"] = *baseline;
  report["matches_baseline"] = (report["current"] == report["baseline"]);
  return report;
}

}  // namespace win_ai_agent::nexus_pro
