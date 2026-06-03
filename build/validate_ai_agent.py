from pathlib import Path
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
header = (root / "include" / "logistic_regression.hpp").read_text()
implementation = (root / "src" / "logistic_regression.cpp").read_text()
main = (root / "src" / "main.cpp").read_text()
readme = (root / "README.md").read_text()
project = (root / "winagent-ai.vcxproj").read_text()
ET.parse(root / "winagent-ai.vcxproj")

required_tokens = [
    "class LogisticRegression",
    "fit(const std::vector<std::vector<double>>& x",
    "predict_proba",
    "cost_for_one",
    "cost_for_zero",
    "1.0 / (1.0 + std::exp(-clipped))",
    "cost += -static_cast<double>(y[row_index]) * std::log(clipped)",
    "--agent-once",
    "--cost-curve",
]

combined = header + implementation + main
for token in required_tokens:
    if token not in combined:
        raise SystemExit(f"missing AI agent token: {token}")

for forbidden in ["IoCreateDevice", "DriverEntry", "TerminateProcess", "OpenProcess", "windows-service"]:
    if forbidden in combined:
        raise SystemExit(f"forbidden system-level token remains: {forbidden}")

if "Windows 11" not in readme or "offline" not in readme or "No kernel driver" not in readme:
    raise SystemExit("README must document Windows 11 offline user-mode boundary")

if "<ConfigurationType>Application</ConfigurationType>" not in project or "<LanguageStandard>stdcpp17</LanguageStandard>" not in project:
    raise SystemExit("Visual Studio project must be a C++17 Windows application")

print("AI agent validation passed")
