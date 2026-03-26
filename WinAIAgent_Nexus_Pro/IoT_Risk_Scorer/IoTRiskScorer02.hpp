#ifndef WIN_AI_AGENT_NEXUS_PRO_IOT_RISK_SCORER_IOTRISKSCORER02_HPP
#define WIN_AI_AGENT_NEXUS_PRO_IOT_RISK_SCORER_IOTRISKSCORER02_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>


namespace win_ai_agent_nexus_pro::iot_risk_scorer {

class IoTRiskScorer02 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::iot_risk_scorer

#endif
