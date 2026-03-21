#ifndef WIN_AI_AGENT_ADVANCED_LOGON_OPTIMIZATION_ENGINE_HPP
#define WIN_AI_AGENT_ADVANCED_LOGON_OPTIMIZATION_ENGINE_HPP

#include "WinAIAgent/Kernel/KernelInterface.hpp"
#include "WinAIAgent/Network/SecureCommunicator.hpp"

#include <expected>
#include <shared_mutex>
#include <string>

namespace win_ai_agent::advanced {

class LogonOptimizationEngine final {
 public:
  LogonOptimizationEngine(const win_ai_agent::kernel::WinKernelManager& kernel,
                          const win_ai_agent::network::SecureCommunicator& communicator);

  std::expected<void, std::string> CollectAndSendBootBottlenecks() const;

 private:
  const win_ai_agent::kernel::WinKernelManager& kernel_;
  const win_ai_agent::network::SecureCommunicator& communicator_;
  mutable std::shared_mutex mutex_;
};

}  // namespace win_ai_agent::advanced

#endif
