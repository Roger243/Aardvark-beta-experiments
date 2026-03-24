#pragma once

#include <nlohmann/json.hpp>

namespace win_ai_agent::nexus_pro {

class TelemetryObserver {
 public:
  virtual ~TelemetryObserver() = default;
  virtual void OnTelemetryEvent(const nlohmann::json& payload) = 0;
};

}  // namespace win_ai_agent::nexus_pro
