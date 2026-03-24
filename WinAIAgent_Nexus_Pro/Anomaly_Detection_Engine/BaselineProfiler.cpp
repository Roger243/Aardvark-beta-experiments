#include "Anomaly_Detection_Engine/BaselineProfiler.hpp"

namespace win_ai_agent_nexus_pro::anomaly_detection_engine {

std::expected<void, std::string> BaselineProfiler::Initialize() {
  return {};
}

std::expected<std::any, std::string> BaselineProfiler::Execute(const std::any& input) {
  return input;
}

}  // namespace win_ai_agent_nexus_pro::anomaly_detection_engine
