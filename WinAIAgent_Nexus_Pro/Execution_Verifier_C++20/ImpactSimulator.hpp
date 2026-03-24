#ifndef WIN_AI_AGENT_NEXUS_PRO_EXECUTION_VERIFIER_CPLUSPLUS20_IMPACTSIMULATOR_HPP
#define WIN_AI_AGENT_NEXUS_PRO_EXECUTION_VERIFIER_CPLUSPLUS20_IMPACTSIMULATOR_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace win_ai_agent_nexus_pro::execution_verifier_cplusplus20 {

struct SystemEvent final {
  std::string domain;
  nlohmann::json payload;
};

using TaskNode = std::variant<std::string, SystemEvent>;

class ImpactSimulator final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const TaskNode& node) const;
};

}  // namespace win_ai_agent_nexus_pro::execution_verifier_cplusplus20

#endif
