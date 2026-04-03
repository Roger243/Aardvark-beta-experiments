#ifndef WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_THREADED_TASK_MANAGER_HPP
#define WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_THREADED_TASK_MANAGER_HPP

#include "WinAIAgent/CoreEngine/ITaskManager.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

namespace win_ai_agent::core {

class ThreadedTaskManager final : public ITaskManager {
 public:
  ThreadedTaskManager();
  ~ThreadedTaskManager() override;

  bool Submit(TaskSpec task) override;
  void Shutdown() override;

 private:
  void WorkerLoop();

  std::atomic<bool> running_{true};
  std::mutex lock_;
  std::condition_variable cv_;
  std::queue<TaskSpec> queue_;
  std::thread worker_;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_THREADED_TASK_MANAGER_HPP
