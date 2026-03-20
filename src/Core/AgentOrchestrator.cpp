#include "WinAIAgent/Core/AgentOrchestrator.hpp"

#include "WinAIAgent/AILogic/AILogicEngine.hpp"
#include "WinAIAgent/Advanced/CredentialProviderBridge.hpp"
#include "WinAIAgent/Advanced/LogonOptimizationEngine.hpp"
#include "WinAIAgent/Advanced/ProcessInjectorCore.hpp"
#include "WinAIAgent/Advanced/SystemTokenStealer.hpp"
#include "WinAIAgent/Advanced/WinlogonUIOverlay.hpp"
#include "WinAIAgent/Kernel/KernelInterface.hpp"
#include "WinAIAgent/Network/NetworkManager.hpp"
#include "WinAIAgent/Network/SecureCommunicator.hpp"
#include "WinAIAgent/Security/CredentialVault.hpp"
#include "WinAIAgent/Security/ScriptExecutor.hpp"

#include <chrono>
#include <expected>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <stop_token>
#include <string>
#include <thread>

namespace win_ai_agent::core {

std::expected<void, std::string> AgentOrchestrator::InitializeSystem() {
  using namespace std::chrono_literals;

  // Module construction
  win_ai_agent::security::CredentialVault credential_vault;
  win_ai_agent::advanced::SystemTokenStealer token_guard;
  win_ai_agent::kernel::WinKernelManager kernel;
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
