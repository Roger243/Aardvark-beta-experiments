#pragma once

#include <string>

namespace win_ai_agent::nexus_pro {

struct UsbDeviceEvent {
  std::string device_instance_id;
  std::string hardware_id;
  bool allowlisted{false};
  std::string risk_level;
};

}  // namespace win_ai_agent::nexus_pro
