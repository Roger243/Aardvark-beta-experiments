#include "Redundant_Service_Nodes/RedundantServiceNodesNode01.hpp"

#include <windows.h>
#include <vector>

namespace win_ai_agent_nexus_pro::redundant_service_nodes {

std::expected<nlohmann::json, std::string> RedundantServiceNodesNode01::Execute(
    const nlohmann::json& input) {
  const std::wstring exe = L"C:\\Program Files\\WinAIAgent\\ai_agent_secondary.exe";
  auto launched = LaunchSecondary(exe);

  nlohmann::json out = input;
  out["module"] = "RedundantServiceNodesNode01";
  out["healthy"] = healthy_.load();
  out["secondary_launch"] = launched.has_value() ? "ok" : launched.error();
  return out;
}

std::expected<void, std::string> RedundantServiceNodesNode01::LaunchSecondary(
    const std::wstring& executable_path) const {
  STARTUPINFOW si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};

  std::wstring cmd = L"\"" + executable_path + L"\" --secondary";
  std::vector<wchar_t> mutable_cmd(cmd.begin(), cmd.end());
  mutable_cmd.push_back(L'\0');

  if (!CreateProcessW(nullptr, mutable_cmd.data(), nullptr, nullptr, FALSE,
                      CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    return std::unexpected("CreateProcessW failed for secondary node");
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);
  return {};
}

}  // namespace win_ai_agent_nexus_pro::redundant_service_nodes
