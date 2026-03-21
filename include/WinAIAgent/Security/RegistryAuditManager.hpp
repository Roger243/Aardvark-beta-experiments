#ifndef WIN_AI_AGENT_SECURITY_REGISTRY_AUDIT_MANAGER_HPP
#define WIN_AI_AGENT_SECURITY_REGISTRY_AUDIT_MANAGER_HPP

#include <nlohmann/json.hpp>

#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace win_ai_agent::security {

struct RegistryChangeEvent final {
  std::string timestamp_utc;
  std::wstring key_path;
  std::string change_type;
};

using RegistryBaseline =
    std::unordered_map<std::wstring, std::unordered_set<std::wstring>>;

class RegistryAuditManager final {
 public:
  RegistryAuditManager();
  ~RegistryAuditManager();

  std::expected<void, std::string> Initialize(std::vector<std::wstring> key_paths);
  std::expected<std::vector<RegistryChangeEvent>, std::string> PollChangesOnce(DWORD timeout_ms);

  nlohmann::json ToJson(const RegistryChangeEvent& event) const;
  nlohmann::json ToJsonBatch(const std::vector<RegistryChangeEvent>& events) const;

  std::expected<nlohmann::json, std::string> DetectDrift(
      const RegistryBaseline& approved_baseline) const;

 private:
  struct WatchedKey final {
    HKEY handle{};
    std::wstring path;
    HANDLE event{};
  };

  std::expected<void, std::string> EmitEtw(std::string_view message) const;
  std::expected<std::unordered_set<std::wstring>, std::string> EnumerateValueNames(
      const std::wstring& key_path) const;

  std::vector<WatchedKey> watched_;
  REGHANDLE etw_handle_{};
};

}  // namespace win_ai_agent::security

#endif
