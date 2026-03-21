#include "JIT_Code_Generator_Bridge/JIT_Code_Generator_BridgeCore.hpp"

namespace win_ai_agent_nexus_pro::jit_code_generator_bridge {

std::expected<std::any, std::string> JIT_Code_Generator_BridgeCore::Execute(const std::any& input) {
  std::scoped_lock lock(mutex_);
  return std::unexpected("Live machine-code generation is disabled in safety-first build");
}

}  // namespace win_ai_agent_nexus_pro::jit_code_generator_bridge
