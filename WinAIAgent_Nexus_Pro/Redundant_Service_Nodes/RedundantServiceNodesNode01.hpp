#ifndef WIN_AI_AGENT_NEXUS_PRO_REDUNDANT_SERVICE_NODES_REDUNDANTSERVICENODESNODE01_HPP
#define WIN_AI_AGENT_NEXUS_PRO_REDUNDANT_SERVICE_NODES_REDUNDANTSERVICENODESNODE01_HPP

#include <atomic>
#include <expected>
#include <nlohmann/json.hpp>
#include <string>

namespace win_ai_agent_nexus_pro::redundant_service_nodes {

/**
 * @brief Primary/secondary launcher for high-availability service topology.
 */
class RedundantServiceNodesNode01 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input);

 private:
  std::expected<void, std::string> LaunchSecondary(const std::wstring& executable_path) const;
  std::atomic<bool> healthy_{true};
};

}  // namespace win_ai_agent_nexus_pro::redundant_service_nodes

#endif
