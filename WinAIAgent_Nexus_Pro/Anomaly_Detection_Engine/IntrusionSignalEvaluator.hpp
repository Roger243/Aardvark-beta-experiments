#ifndef WIN_AI_AGENT_NEXUS_PRO_ANOMALY_DETECTION_ENGINE_INTRUSIONSIGNALEVALUATOR_HPP
#define WIN_AI_AGENT_NEXUS_PRO_ANOMALY_DETECTION_ENGINE_INTRUSIONSIGNALEVALUATOR_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::anomaly_detection_engine {

/**
 * @brief IntrusionSignalEvaluator module for WinAIAgent_Nexus_Pro.
 */
class IntrusionSignalEvaluator final {
 public:
  std::expected<void, std::string> Initialize();
  std::expected<std::any, std::string> Execute(const std::any& input);
};

}  // namespace win_ai_agent_nexus_pro::anomaly_detection_engine

#endif
