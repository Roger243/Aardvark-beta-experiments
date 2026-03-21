#ifndef WIN_AI_AGENT_NEXUS_PRO_REASONING_CONTEXT_BUILDER_REASONINGCONTEXTBUILDER01_HPP
#define WIN_AI_AGENT_NEXUS_PRO_REASONING_CONTEXT_BUILDER_REASONINGCONTEXTBUILDER01_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>


namespace win_ai_agent_nexus_pro::reasoning_context_builder {

class ReasoningContextBuilder01 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::reasoning_context_builder

#endif
