#include "WinAIAgent/Security/RegistryAuditManager.hpp"

#include <windows.h>
#include <evntprov.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace win_ai_agent::security {
namespace {

constexpr GUID kProviderGuid = {0xc57a9241,
                                0x9314,
                                0x4f6a,
                                {0x93, 0xb5, 0x2b, 0xd8, 0x4d, 0x5c, 0x9d, 0x42}};

std::string UtcNowIso8601() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  std::tm utc_tm{};
#if defined(_WIN32)
  gmtime_s(&utc_tm, &now_time);
#else
  gmtime_r(&now_time, &utc_tm);
#endif

  std::ostringstream out;
  out << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
  return out.str();
}

std::expected<std::pair<HKEY, std::wstring>, std::string> ParseHive(
    const std::wstring& full_path) {
  constexpr std::wstring_view hklm = L"HKEY_LOCAL_MACHINE\\";
  if (full_path.rfind(hklm.data(), 0) == 0) {
    return std::make_pair(HKEY_LOCAL_MACHINE,
                          full_path.substr(hklm.size()));
  }
  return std::unexpected("Only HKEY_LOCAL_MACHINE paths are supported");
}

}  // namespace

RegistryAuditManager::RegistryAuditManager() {
  EventRegister(&kProviderGuid, nullptr, nullptr, &etw_handle_);
}

RegistryAuditManager::~RegistryAuditManager() {
  for (auto& watch : watched_) {
    if (watch.event != nullptr) {
      CloseHandle(watch.event);
      watch.event = nullptr;
    }
    if (watch.handle != nullptr) {
      RegCloseKey(watch.handle);
      watch.handle = nullptr;
    }
  }

  if (etw_handle_ != 0) {
    EventUnregister(etw_handle_);
    etw_handle_ = 0;
  }
}

std::expected<void, std::string> RegistryAuditManager::Initialize(
    std::vector<std::wstring> key_paths) {
  for (const auto& full_path : key_paths) {
    auto hive_parse = ParseHive(full_path);
    if (!hive_parse) {
      return std::unexpected(hive_parse.error());
    }

    HKEY key = nullptr;
    const auto& [root, subkey] = *hive_parse;
    const LONG rc = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_NOTIFY | KEY_QUERY_VALUE,
                                  &key);
    if (rc != ERROR_SUCCESS) {
      return std::unexpected("RegOpenKeyExW failed while initializing watch");
    }

    HANDLE evt = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (evt == nullptr) {
      RegCloseKey(key);
      return std::unexpected("CreateEventW failed for registry watcher");
    }

    watched_.push_back(WatchedKey{.handle = key, .path = full_path, .event = evt});

    const LONG notify_rc = RegNotifyChangeKeyValue(
        key, TRUE,
        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY,
        evt, TRUE);
    if (notify_rc != ERROR_SUCCESS) {
      return std::unexpected("RegNotifyChangeKeyValue failed during initialization");
    }
  }

  return EmitEtw("RegistryAuditManager initialized");
}

std::expected<std::vector<RegistryChangeEvent>, std::string>
RegistryAuditManager::PollChangesOnce(DWORD timeout_ms) {
  if (watched_.empty()) {
    return std::unexpected("RegistryAuditManager is not initialized");
  }

  std::vector<HANDLE> events;
  events.reserve(watched_.size());
  for (const auto& w : watched_) {
    events.push_back(w.event);
  }

  const DWORD wait = WaitForMultipleObjects(static_cast<DWORD>(events.size()),
                                            events.data(), FALSE, timeout_ms);

  std::vector<RegistryChangeEvent> detected;
  if (wait >= WAIT_OBJECT_0 && wait < WAIT_OBJECT_0 + events.size()) {
    const std::size_t index = wait - WAIT_OBJECT_0;
    auto& watch = watched_[index];

    detected.push_back(RegistryChangeEvent{.timestamp_utc = UtcNowIso8601(),
                                           .key_path = watch.path,
                                           .change_type = "registry_change"});

    const LONG rearm = RegNotifyChangeKeyValue(
        watch.handle, TRUE,
        REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY,
        watch.event, TRUE);
    if (rearm != ERROR_SUCCESS) {
      return std::unexpected("RegNotifyChangeKeyValue failed while rearming watcher");
    }

    const auto payload = ToJsonBatch(detected).dump();
    (void)EmitEtw(payload);
  }

  return detected;
}

nlohmann::json RegistryAuditManager::ToJson(const RegistryChangeEvent& event) const {
  return {
      {"timestamp_utc", event.timestamp_utc},
      {"key_path", std::string(event.key_path.begin(), event.key_path.end())},
      {"change_type", event.change_type},
  };
}

nlohmann::json RegistryAuditManager::ToJsonBatch(
    const std::vector<RegistryChangeEvent>& events) const {
  nlohmann::json out = nlohmann::json::array();
  for (const auto& event : events) {
    out.push_back(ToJson(event));
  }
  return out;
}

std::expected<nlohmann::json, std::string> RegistryAuditManager::DetectDrift(
    const RegistryBaseline& approved_baseline) const {
  nlohmann::json deviations = nlohmann::json::array();

  for (const auto& [path, approved_values] : approved_baseline) {
    auto current_values = EnumerateValueNames(path);
    if (!current_values) {
      return std::unexpected(current_values.error());
    }

    for (const auto& current : *current_values) {
      if (!approved_values.contains(current)) {
        deviations.push_back({
            {"key_path", std::string(path.begin(), path.end())},
            {"value_name", std::string(current.begin(), current.end())},
            {"drift_type", "unexpected_value"},
        });
      }
    }

    for (const auto& approved : approved_values) {
      if (!current_values->contains(approved)) {
        deviations.push_back({
            {"key_path", std::string(path.begin(), path.end())},
            {"value_name", std::string(approved.begin(), approved.end())},
            {"drift_type", "missing_value"},
        });
      }
    }
  }

  return deviations;
}

std::expected<void, std::string> RegistryAuditManager::EmitEtw(
    std::string_view message) const {
  std::wstring wide(message.begin(), message.end());
  const ULONG rc = EventWriteString(etw_handle_, 0, 0, wide.c_str());
  if (rc != ERROR_SUCCESS) {
    return std::unexpected("EventWriteString failed");
  }
  return {};
}

std::expected<std::unordered_set<std::wstring>, std::string>
RegistryAuditManager::EnumerateValueNames(const std::wstring& key_path) const {
  auto parsed = ParseHive(key_path);
  if (!parsed) {
    return std::unexpected(parsed.error());
  }

  HKEY key = nullptr;
  const auto& [root, subkey] = *parsed;
  const LONG rc = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_QUERY_VALUE, &key);
  if (rc != ERROR_SUCCESS) {
    return std::unexpected("RegOpenKeyExW failed in EnumerateValueNames");
  }

  std::unordered_set<std::wstring> values;
  DWORD index = 0;
  wchar_t name[256];
  DWORD name_len = static_cast<DWORD>(std::size(name));
  while (RegEnumValueW(key, index, name, &name_len, nullptr, nullptr, nullptr, nullptr) ==
         ERROR_SUCCESS) {
    values.insert(std::wstring(name, name_len));
    ++index;
    name_len = static_cast<DWORD>(std::size(name));
  }

  RegCloseKey(key);
  return values;
}

}  // namespace win_ai_agent::security
