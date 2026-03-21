#ifndef WIN_AI_AGENT_AI_LOGIC_AI_LOGIC_ENGINE_HPP
#define WIN_AI_AGENT_AI_LOGIC_AI_LOGIC_ENGINE_HPP

#include <nlohmann/json.hpp>

#include <string>

namespace win_ai_agent::ai_logic {

class AILogicEngine final {
 public:
  nlohmann::json BuildPromptEnvelope(const std::string& telemetry_json) const;
};

}  // namespace win_ai_agent::ai_logic

#endif  // WIN_AI_AGENT_AI_LOGIC_AI_LOGIC_ENGINE_HPP
