#include "src/core/telemetry/UsbTelemetryMonitor.hpp"

#ifdef _WIN32
#include <windows.h>
#include <dbt.h>
#include <setupapi.h>
#endif

namespace win_ai_agent::nexus_pro {

std::expected<void, WinErrorStruct> UsbTelemetryMonitor::Start(std::stop_token token) {
#ifdef _WIN32
  while (!token.stop_requested()) {
    ::Sleep(200);
  }
  return {};
#else
  (void)token;
  return std::unexpected(MakeWinError("WM_DEVICECHANGE monitor"));
#endif
}

void UsbTelemetryMonitor::Subscribe(std::shared_ptr<TelemetryObserver> observer) {
  observers_.push_back(std::move(observer));
}

UsbDeviceEvent UsbTelemetryMonitor::ClassifyEvent(const std::string& instance_id,
                                                  const std::string& hardware_id) const {
  const bool allowed = allowlist_.IsAllowed(hardware_id);
  return UsbDeviceEvent{instance_id, hardware_id, allowed, allowed ? "Low" : "High Risk"};
}

}  // namespace win_ai_agent::nexus_pro
