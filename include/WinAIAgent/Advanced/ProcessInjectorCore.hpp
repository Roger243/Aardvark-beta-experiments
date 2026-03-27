#ifndef WIN_AI_AGENT_ADVANCED_PROCESS_INJECTOR_CORE_HPP
#define WIN_AI_AGENT_ADVANCED_PROCESS_INJECTOR_CORE_HPP

#include <expected>
#include <shared_mutex>
#include <string>

namespace win_ai_agent::advanced {

class ProcessInjectorCore final {
 public:
  std::expected<void, std::string> MonitorSessionProcessesSafely();

 private:
  mutable std::shared_mutex mutex_;
};

}  // namespace win_ai_agent::advanced

#endif
