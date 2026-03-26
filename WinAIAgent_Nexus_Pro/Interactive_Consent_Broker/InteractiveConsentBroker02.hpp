#ifndef WIN_AI_AGENT_NEXUS_PRO_INTERACTIVE_CONSENT_BROKER_INTERACTIVECONSENTBROKER02_HPP
#define WIN_AI_AGENT_NEXUS_PRO_INTERACTIVE_CONSENT_BROKER_INTERACTIVECONSENTBROKER02_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>


namespace win_ai_agent_nexus_pro::interactive_consent_broker {

class InteractiveConsentBroker02 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::interactive_consent_broker

#endif
