#ifndef WIN_AI_AGENT_NEXUS_PRO_CLOUD_BUDGET_OBSERVER_CLOUDBUDGETOBSERVER01_HPP
#define WIN_AI_AGENT_NEXUS_PRO_CLOUD_BUDGET_OBSERVER_CLOUDBUDGETOBSERVER01_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>


namespace win_ai_agent_nexus_pro::cloud_budget_observer {

class CloudBudgetObserver01 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::cloud_budget_observer

#endif
