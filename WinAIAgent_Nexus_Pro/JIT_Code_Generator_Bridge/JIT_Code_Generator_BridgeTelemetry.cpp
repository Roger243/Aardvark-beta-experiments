#include "JIT_Code_Generator_Bridge/JIT_Code_Generator_BridgeTelemetry.hpp"

namespace win_ai_agent_nexus_pro::jit_code_generator_bridge {

std::expected<std::any, std::string> JIT_Code_Generator_BridgeTelemetry::Execute(const std::any& input) {
  std::scoped_lock lock(mutex_);
  return input;
}

}  // namespace win_ai_agent_nexus_pro::jit_code_generator_bridge
