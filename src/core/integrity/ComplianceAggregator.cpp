#include "src/core/integrity/ComplianceAggregator.hpp"

#include "src/core/integrity/IntegrityManifest.hpp"

namespace win_ai_agent::nexus_pro {

void ComplianceAggregator::AddStrategy(std::shared_ptr<IntegrityAuditStrategy> strategy) {
  strategies_.push_back(std::move(strategy));
}

std::expected<nlohmann::json, WinErrorStruct> ComplianceAggregator::Collect() const {
  auto manifest = IntegrityManifest::CreateEmpty();
  std::size_t anomalies = 0;

  for (const auto& strategy : strategies_) {
    const auto report = strategy->RunAudit();
    if (!report) {
      return std::unexpected(report.error());
    }
    manifest["modules"].push_back(*report);
    if (report->value("matches_baseline", true) == false ||
        report->value("testsigning_on", false) ||
        report->value("nointegritychecks_on", false)) {
      ++anomalies;
    }
  }

  const double total = strategies_.empty() ? 1.0 : static_cast<double>(strategies_.size());
  manifest["anomaly_score"] = anomalies / total;
  return manifest;
}

}  // namespace win_ai_agent::nexus_pro
