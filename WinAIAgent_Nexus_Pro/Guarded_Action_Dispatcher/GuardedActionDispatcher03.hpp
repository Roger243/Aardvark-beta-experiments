#ifndef WIN_AI_AGENT_NEXUS_PRO_GUARDED_ACTION_DISPATCHER_GUARDEDACTIONDISPATCHER03_HPP
#define WIN_AI_AGENT_NEXUS_PRO_GUARDED_ACTION_DISPATCHER_GUARDEDACTIONDISPATCHER03_HPP

#include <expected>
#include <nlohmann/json.hpp>
#include <string>


namespace win_ai_agent_nexus_pro::guarded_action_dispatcher {

class GuardedActionDispatcher03 final {
 public:
  std::expected<nlohmann::json, std::string> Execute(const nlohmann::json& input) const;
};

}  // namespace win_ai_agent_nexus_pro::guarded_action_dispatcher

#endif
