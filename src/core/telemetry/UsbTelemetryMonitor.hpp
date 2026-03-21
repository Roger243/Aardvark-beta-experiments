#pragma once

#include "src/core/telemetry/TelemetryObserver.hpp"
#include "src/core/telemetry/UsbAllowlist.hpp"
#include "src/core/telemetry/UsbTypes.hpp"
#include "src/utils/WinApiTypes.hpp"

#include <expected>
#include <jthread>
#include <memory>
#include <vector>

namespace win_ai_agent::nexus_pro {

class UsbTelemetryMonitor {
 public:
  [[nodiscard]] std::expected<void, WinErrorStruct> Start(std::stop_token token);
  void Subscribe(std::shared_ptr<TelemetryObserver> observer);
  [[nodiscard]] UsbDeviceEvent ClassifyEvent(const std::string& instance_id, const std::string& hardware_id) const;

 private:
  std::vector<std::shared_ptr<TelemetryObserver>> observers_;
  UsbAllowlist allowlist_;
};

}  // namespace win_ai_agent::nexus_pro
