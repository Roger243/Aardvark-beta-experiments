#ifndef WIN_AI_AGENT_NEXUS_PRO_JIT_CODE_GENERATOR_BRIDGE_JIT_CODE_GENERATOR_BRIDGECORE_HPP
#define WIN_AI_AGENT_NEXUS_PRO_JIT_CODE_GENERATOR_BRIDGE_JIT_CODE_GENERATOR_BRIDGECORE_HPP

#include <any>
#include <expected>
#include <mutex>
#include <string>

namespace win_ai_agent_nexus_pro::jit_code_generator_bridge {

class JIT_Code_Generator_BridgeCore final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input);

 private:
  std::mutex mutex_;
};

}  // namespace win_ai_agent_nexus_pro::jit_code_generator_bridge

#endif
