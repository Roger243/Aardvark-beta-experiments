#include "WinAIAgent/Core/AgentOrchestrator.hpp"

#include <windows.h>

#include <cstdlib>
#include <iostream>

namespace {

bool ShowEULAAgreement() {
  constexpr const char* kDisclaimer =
      "WinAIAgent Research Disclaimer\n\n"
      "This software is for controlled engineering research environments only.\n"
      "It may collect system telemetry and run constrained automation workflows.\n"
      "Do not run on unmanaged production or personal endpoints.\n\n"
      "Click OK to indicate: I AGREE.\n"
      "Click Cancel to exit.";

  const int rc = MessageBoxA(nullptr, kDisclaimer, "WinAIAgent EULA",
                             MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2);
  if (rc == IDCANCEL) {
    std::exit(0);
  }
  return rc == IDOK;
}

}  // namespace

int main() {
  win_ai_agent::core::AgentOrchestrator orchestrator;

  if (!orchestrator.CheckIfEngineerEnvironment()) {
    std::exit(1);
  }

  if (!ShowEULAAgreement()) {
    std::exit(0);
  }

  auto init = orchestrator.InitializeSystem();
  if (!init) {
    std::cerr << "Initialization failed: " << init.error() << '\n';
    return 1;
  }

  orchestrator.Run();
  return 0;
}
