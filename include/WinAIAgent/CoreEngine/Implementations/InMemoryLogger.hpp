#ifndef WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_IN_MEMORY_LOGGER_HPP
#define WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_IN_MEMORY_LOGGER_HPP

#include "WinAIAgent/CoreEngine/ILogger.hpp"

#include <chrono>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace win_ai_agent::core {

struct LogEntry final {
  std::chrono::system_clock::time_point timestamp{};
  LogLevel level{LogLevel::kInfo};
  std::string component;
  std::string message;
};

class InMemoryLogger final : public ILogger {
 public:
  explicit InMemoryLogger(std::size_t max_entries = 2000);

  void Log(LogLevel level, std::string_view component, std::string_view message) override;
  [[nodiscard]] std::vector<LogEntry> Snapshot() const;

 private:
  std::size_t max_entries_{2000};
  mutable std::mutex lock_;
  std::deque<LogEntry> entries_;
};

}  // namespace win_ai_agent::core

#endif  // WIN_AI_AGENT_CORE_ENGINE_IMPLEMENTATIONS_IN_MEMORY_LOGGER_HPP
