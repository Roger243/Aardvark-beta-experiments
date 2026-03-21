#include "WinAIAgent/AILogic/AILogicEngine.hpp"

namespace win_ai_agent::ai_logic {

nlohmann::json AILogicEngine::BuildPromptEnvelope(const std::string& telemetry_json) const {
  return {
      {"model", "gpt-4.1"},
      {"input", telemetry_json},
      {"safety", "no-raw-code-exec"},
  };
}

}  // namespace win_ai_agent::ai_logic
