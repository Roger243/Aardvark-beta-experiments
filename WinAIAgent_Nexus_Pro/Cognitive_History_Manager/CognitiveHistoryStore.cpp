#include "Cognitive_History_Manager/CognitiveHistoryStore.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

#if __has_include(<sqlite3.h>)
#include <sqlite3.h>
#define WIN_AI_NEXUS_HAS_SQLITE 1
#else
#define WIN_AI_NEXUS_HAS_SQLITE 0
#endif

namespace win_ai_agent_nexus_pro::cognitive_history_manager {

std::expected<void, std::string> CognitiveHistoryStore::Initialize() {
#if WIN_AI_NEXUS_HAS_SQLITE
  sqlite3* db = nullptr;
  if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
    return std::unexpected("sqlite3_open failed");
  }

  const char* ddl =
      "CREATE TABLE IF NOT EXISTS experience(action_id TEXT, state_json TEXT, outcome TEXT, "
      "feedback REAL, ts DATETIME DEFAULT CURRENT_TIMESTAMP);";
  char* err = nullptr;
  if (sqlite3_exec(db, ddl, nullptr, nullptr, &err) != SQLITE_OK) {
    std::string error = err == nullptr ? "sqlite3_exec failed" : err;
    sqlite3_free(err);
    sqlite3_close(db);
    return std::unexpected(error);
  }
  sqlite3_close(db);
#else
  std::ofstream out(fallback_file_, std::ios::app);
  if (!out) {
    return std::unexpected("Failed to open fallback flat-file store");
  }
#endif
  return {};
}

std::expected<std::any, std::string> CognitiveHistoryStore::Execute(const std::any& input) {
  if (!input.has_value()) {
    return std::unexpected("CognitiveHistoryStore::Execute expects a JSON payload");
  }

  if (input.type() != typeid(nlohmann::json)) {
    return std::unexpected("Input must be nlohmann::json");
  }

  const auto payload = std::any_cast<nlohmann::json>(input);

#if WIN_AI_NEXUS_HAS_SQLITE
  sqlite3* db = nullptr;
  if (sqlite3_open(db_path_.c_str(), &db) != SQLITE_OK) {
    return std::unexpected("sqlite3_open failed while writing");
  }

  const std::string sql =
      "INSERT INTO experience(action_id, state_json, outcome, feedback) VALUES(?1, ?2, ?3, ?4);";
  sqlite3_stmt* stmt = nullptr;
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    sqlite3_close(db);
    return std::unexpected("sqlite3_prepare_v2 failed");
  }

  sqlite3_bind_text(stmt, 1, payload.value("action_id", "").c_str(), -1, SQLITE_TRANSIENT);
  const std::string state = payload.value("state_json", "{}");
  sqlite3_bind_text(stmt, 2, state.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, payload.value("outcome", "unknown").c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 4, payload.value("feedback", 0.0));

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return std::unexpected("sqlite3_step failed");
  }
  sqlite3_finalize(stmt);
  sqlite3_close(db);
#else
  std::ofstream out(fallback_file_, std::ios::app);
  if (!out) {
    return std::unexpected("Failed to write fallback flat-file store");
  }
  out << payload.dump() << "\n";
#endif

  return payload;
}

}  // namespace win_ai_agent_nexus_pro::cognitive_history_manager
