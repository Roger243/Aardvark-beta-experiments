#include "Dynamic_Plugin_Loader/Dynamic_Plugin_LoaderCore.hpp"

namespace win_ai_agent_nexus_pro::dynamic_plugin_loader {

std::expected<std::any, std::string> Dynamic_Plugin_LoaderCore::Execute(const std::any& input) {
  std::scoped_lock lock(mutex_);
  return input;
}

}  // namespace win_ai_agent_nexus_pro::dynamic_plugin_loader
