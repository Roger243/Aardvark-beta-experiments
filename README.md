# 🧠 WinAIAgent

**WinAIAgent** is a research-oriented C++20 framework that explores how **LLM-guided reasoning** can be connected to **Windows internals telemetry and control planes** for safe, auditable OS management workflows.

> Think of it as a bridge between modern AI orchestration and low-level Windows observability.

---

## ⚠️ Safety & Ethics Warning (Read First)

**This is RESEARCH SOFTWARE.**

- Run only in **isolated lab environments** (VMs, snapshots, sandboxes).
- Do **not** deploy to production endpoints.
- Misconfiguration can cause instability, service disruption, or data corruption.
- The maintainers/authors are **not responsible** for BSODs, data loss, or unintended side effects.

Additionally:

- Any offensive behavior (e.g., stealth process injection, token theft, covert persistence) is out of scope for responsible usage.
- This codebase intentionally includes **policy guardrails** that reject dangerous privilege abuse paths.

---

## 🧱 Architecture Overview

WinAIAgent uses a **27+ file modular architecture** grouped into the following domains:

- **Core**: Orchestration and lifecycle glue.
- **Kernel**: Process/thread/memory/I/O inspection primitives.
- **Network**: HTTP transport, secure telemetry posting, proxy-aware communication.
- **Security**: Credential storage and controlled script execution.
- **Advanced / Winlogon**: Research interfaces around pre-logon/session workflows.
- **AI Logic**: Prompt envelope and reasoning integration layer.

High-level flow:

1. Collect telemetry from Windows APIs.
2. Normalize and package telemetry as JSON.
3. Send to remote AI services through secure communication layers.
4. Feed reasoning outputs into constrained local execution paths.
5. Record errors and operational diagnostics for auditing.

---

## 🔬 Key Technical Features

### 1) Winlogon / Session Interaction (Research Interface)
- Includes Winlogon-adjacent module boundaries for pre-logon status flows.
- Current implementation favors safe desktop messaging and telemetry bridges over invasive hooks.

### 2) Privilege-Boundary Research
- The repository contains placeholders for privilege-boundary analysis modules.
- **Important:** token theft / unauthorized escalation behaviors are explicitly blocked by design.

### 3) Real-Time Telemetry
- WinAPI-based telemetry pipelines gather process and resource insights.
- Designed to be extended with ETW provider ingestion for deeper boot/logon performance analysis.

### 4) Autonomous Script Pipeline (Constrained)
- PowerShell/CMD execution is available through signed/controlled pathways.
- Output is captured in bounded buffers for debugging and traceability.
- Intended for controlled remediation experiments, not untrusted remote execution.

---

## 📦 Requirements

- **OS**: Windows 10/11 or Windows Server (modern build)
- **Toolchain**:
  - Visual Studio 2022 (MSVC)
  - CMake 3.23+
  - Windows SDK
  - C++20 support
- **Dependencies** (recommended via `vcpkg`):
  - `curl`
  - `nlohmann-json`

Example with vcpkg:

```powershell
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install curl nlohmann-json
```

---

## 🛠️ Build Instructions (CMake)

```powershell
git clone <your-fork-or-repo-url>
cd WinAIAgent

cmake -S . -B build ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build build --config Release
```

If `nlohmann_json` is not discovered, set `CMAKE_PREFIX_PATH` or verify your vcpkg triplet/toolchain configuration.

---

## 🧪 Safe Execution Guide

### Dry-run Mode
A strict **dry-run mode is recommended** for all initial validation sessions.

Suggested behavior for dry-run deployments:
- Disable script execution side effects.
- Disable service registration side effects.
- Keep telemetry + orchestration paths enabled for observability testing.

> If your local branch does not yet expose `--dry-run` CLI parsing, implement it before non-lab execution.

### Register as a Windows Service (Lab only)

```powershell
sc create WinAIAgent binPath= "C:\path\to\ai_agent.exe" start= demand
sc start WinAIAgent
```

Remove service:

```powershell
sc stop WinAIAgent
sc delete WinAIAgent
```

### Monitor Logs (Event Viewer)

1. Open **Event Viewer** (`eventvwr.msc`).
2. Check:
   - **Windows Logs → Application**
   - **Windows Logs → System**
3. Correlate service start/stop events with your WinAIAgent telemetry timestamps.

---

## 🧭 Responsible Research Scope

This project is intended for:

- AI alignment experiments with system-level feedback loops.
- Explainable remediation decisioning.
- Defensive telemetry + controlled automation in sandboxed environments.

This project is **not** intended for:

- Evasion, persistence abuse, credential theft, or unauthorized privilege escalation.

---

## 🤝 Community Call

We invite:

- Windows internals engineers,
- AI safety researchers,
- OpenAI-alignment practitioners,
- defensive security architects,

to review and improve:

1. **AI Alignment controls** (decision boundaries, fail-safe gating), and
2. **Kernel Safety controls** (stability, exception isolation, and rollback posture).

If you contribute, please include:
- threat model assumptions,
- reproducible lab setup,
- and validation evidence (logs/tests/traces).

---

## 📄 License & Contribution Notes

Before publishing or deploying derivatives, ensure compliance with:

- local law,
- organizational security policy,
- platform terms of use,
- and responsible disclosure practices.

