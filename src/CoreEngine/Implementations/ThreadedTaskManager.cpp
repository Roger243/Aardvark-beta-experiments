#include "WinAIAgent/CoreEngine/Implementations/ThreadedTaskManager.hpp"

#include <utility>

namespace win_ai_agent::core {

ThreadedTaskManager::ThreadedTaskManager() : worker_([this] { WorkerLoop(); }) {}

ThreadedTaskManager::~ThreadedTaskManager() { Shutdown(); }

bool ThreadedTaskManager::Submit(TaskSpec task) {
  if (!running_.load()) {
    return false;
  }

  {
    std::lock_guard<std::mutex> guard(lock_);
    queue_.push(std::move(task));
  }
  cv_.notify_one();
  return true;
}

void ThreadedTaskManager::Shutdown() {
  bool expected = true;
  if (!running_.compare_exchange_strong(expected, false)) {
    return;
  }

  cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }
}

void ThreadedTaskManager::WorkerLoop() {
  while (running_.load()) {
    TaskSpec task;
    {
      std::unique_lock<std::mutex> lk(lock_);
      cv_.wait(lk, [this] { return !running_.load() || !queue_.empty(); });
      if (!running_.load() && queue_.empty()) {
        return;
      }
      task = std::move(queue_.front());
      queue_.pop();
    }

    if (task.work) {
      task.work();
    }
  }
}

}  // namespace win_ai_agent::core
