#pragma once

namespace win_ai_agent::nexus_pro {

class ConsentGate {
 public:
  [[nodiscard]] bool IsApprovedByUser() const { return false; }
};

}  // namespace win_ai_agent::nexus_pro
