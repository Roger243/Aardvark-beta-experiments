#ifndef WIN_AI_AGENT_NEXUS_PRO_DYNAMIC_PLUGIN_LOADER_DYNAMIC_PLUGIN_LOADERCONFIG_HPP
#define WIN_AI_AGENT_NEXUS_PRO_DYNAMIC_PLUGIN_LOADER_DYNAMIC_PLUGIN_LOADERCONFIG_HPP

#include <any>
#include <expected>
#include <mutex>
#include <string>

namespace win_ai_agent_nexus_pro::dynamic_plugin_loader {

class Dynamic_Plugin_LoaderConfig final {
 public:
  std::expected<std::any, std::string> Execute(const std::any& input);

 private:
  std::mutex mutex_;
};

}  // namespace win_ai_agent_nexus_pro::dynamic_plugin_loader

#endif
