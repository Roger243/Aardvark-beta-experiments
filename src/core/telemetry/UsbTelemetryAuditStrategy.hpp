#pragma once

#include "src/core/integrity/IntegrityAuditStrategy.hpp"
#include "src/core/telemetry/UsbTelemetryMonitor.hpp"

namespace win_ai_agent::nexus_pro {

class UsbTelemetryAuditStrategy final : public IntegrityAuditStrategy {
 public:
  [[nodiscard]] std::expected<nlohmann::json, WinErrorStruct> RunAudit() override;

 private:
  UsbTelemetryMonitor monitor_;
};

}  // namespace win_ai_agent::nexus_pro
