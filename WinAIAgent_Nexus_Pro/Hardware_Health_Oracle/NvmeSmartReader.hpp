#ifndef WIN_AI_AGENT_NEXUS_PRO_HARDWARE_HEALTH_ORACLE_NVMESMARTREADER_HPP
#define WIN_AI_AGENT_NEXUS_PRO_HARDWARE_HEALTH_ORACLE_NVMESMARTREADER_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::hardware_health_oracle {

class NvmeSmartReader final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input) const;
};

}  // namespace win_ai_agent_nexus_pro::hardware_health_oracle

#endif
