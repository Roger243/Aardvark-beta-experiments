#ifndef WIN_AI_AGENT_NEXUS_PRO_COGNITIVE_HISTORY_MANAGER_COGNITIVEHISTORYSTORE_HPP
#define WIN_AI_AGENT_NEXUS_PRO_COGNITIVE_HISTORY_MANAGER_COGNITIVEHISTORYSTORE_HPP

#include <any>
#include <expected>
#include <string>

namespace win_ai_agent_nexus_pro::cognitive_history_manager {

/**
 * @brief Persistent memory store for action/state/outcome experiences.
 *
 * Uses SQLite3 when available; otherwise falls back to append-only flat-file JSONL.
 */
class CognitiveHistoryStore final {
 public:
  std::expected<void, std::string> Initialize();
  std::expected<std::any, std::string> Execute(const std::any& input);

 private:
  std::string db_path_{"nexus_pro_memory.db"};
  std::string fallback_file_{"nexus_pro_memory.jsonl"};
};

}  // namespace win_ai_agent_nexus_pro::cognitive_history_manager

#endif
