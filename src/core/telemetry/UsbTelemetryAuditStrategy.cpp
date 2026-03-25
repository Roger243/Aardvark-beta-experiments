#include "src/core/telemetry/UsbTelemetryAuditStrategy.hpp"

namespace win_ai_agent::nexus_pro {

std::expected<nlohmann::json, WinErrorStruct> UsbTelemetryAuditStrategy::RunAudit() {
  const auto sample = monitor_.ClassifyEvent("USB\VID_0000&PID_0000", "VID_0000&PID_0000");
  return nlohmann::json{
      {"module", "USB_Telemetry_Monitor"},
      {"sample_device", {{"instance_id", sample.device_instance_id},
                          {"hardware_id", sample.hardware_id},
                          {"allowlisted", sample.allowlisted},
                          {"risk_level", sample.risk_level}}},
  };
}

}  // namespace win_ai_agent::nexus_pro
