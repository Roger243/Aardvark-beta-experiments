#ifndef WIN_AI_AGENT_CORE_ENGINE_I_MODULE_HPP
#define WIN_AI_AGENT_CORE_ENGINE_I_MODULE_HPP

#include <string_view>

namespace win_ai_agent::core {

class IModule {
 public:
  virtual ~IModule() = default;

  [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
  virtual bool Start() = 0;
  virtual void Stop() noexcept = 0;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_I_MODULE_HPP
