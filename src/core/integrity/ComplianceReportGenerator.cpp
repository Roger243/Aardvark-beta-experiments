#include "src/core/integrity/ComplianceReportGenerator.hpp"

#include <fstream>

namespace win_ai_agent::nexus_pro {

ComplianceReportGenerator::ComplianceReportGenerator(ComplianceAggregator aggregator)
    : aggregator_(std::move(aggregator)) {}

std::expected<void, WinErrorStruct> ComplianceReportGenerator::WriteIntegrityManifest(
    const std::string& output_path) const {
  const auto report = aggregator_.Collect();
  if (!report) {
    return std::unexpected(report.error());
  }

  std::ofstream out(output_path, std::ios::trunc);
  if (!out) {
    return std::unexpected(WinErrorStruct{5, "Unable to write IntegrityManifest.json"});
  }

  out << report->dump(2);
  return {};
}

}  // namespace win_ai_agent::nexus_pro
