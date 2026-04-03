#include "WinAIAgent/CoreEngine/Implementations/InMemoryLogger.hpp"

#include <utility>

namespace win_ai_agent::core {

InMemoryLogger::InMemoryLogger(const std::size_t max_entries)
    : max_entries_(max_entries == 0 ? 1 : max_entries) {}

void InMemoryLogger::Log(const LogLevel level, const std::string_view component,
                        const std::string_view message) {
  std::lock_guard<std::mutex> guard(lock_);
  entries_.push_back(LogEntry{
      .timestamp = std::chrono::system_clock::now(),
      .level = level,
      .component = std::string(component),
      .message = std::string(message),
  });

  while (entries_.size() > max_entries_) {
    entries_.pop_front();
  }
}

std::vector<LogEntry> InMemoryLogger::Snapshot() const {
  std::lock_guard<std::mutex> guard(lock_);
  return std::vector<LogEntry>(entries_.begin(), entries_.end());
}

}  // namespace win_ai_agent::core
