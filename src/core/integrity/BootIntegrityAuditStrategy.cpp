#include "src/core/integrity/BootIntegrityAuditStrategy.hpp"

#include "src/core/integrity/BcdValueConverter.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> BootIntegrityAuditStrategy::RunAudit() {
  const auto elements = reader_.ReadBootManagerElements();
  if (!elements) {
    return std::unexpected(elements.error());
  }

  nlohmann::json report;
  report["module"] = "Boot_Integrity_Auditor";
  report["read_only"] = true;
  report["elements"] = nlohmann::json::array();

  bool testsigning = false;
  bool nointegrity = false;

  for (const auto& e : *elements) {
    report["elements"].push_back({
        {"type", e.type},
        {"description", e.description},
        {"raw_value", e.raw_value},
        {"value_text", BcdValueConverter::FlagValueToString(e.raw_value)},
    });
    if (e.type == 0x16000048 && e.raw_value != 0) testsigning = true;
    if (e.type == 0x16000049 && e.raw_value != 0) nointegrity = true;
  }

  report["testsigning_on"] = testsigning;
  report["nointegritychecks_on"] = nointegrity;
  return report;
}

}  // namespace win_ai_agent::nexus_pro
