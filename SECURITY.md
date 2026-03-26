# SECURITY POLICY

## Scope

**WinAIAgent** is a high-privilege C++20 research framework intended for controlled Windows security and AI-alignment experimentation.

Because this codebase can interact with sensitive operating-system surfaces, all contributors and researchers must follow strict security and ethical standards.

---

## Vulnerability Disclosure Policy

If you discover a security issue, please report it responsibly:

1. **Preferred (private):** Email the maintainers/security contact with:
   - vulnerability description,
   - affected files/modules,
   - impact assessment,
   - reproduction steps,
   - proof-of-concept details (if safe).
2. **Alternative (public):** Open a GitHub Issue **only** for non-sensitive findings.

Please do **not** publish 0-day exploit details until maintainers have had a reasonable remediation window.

---

## Mandatory Security Standards

This project may run with elevated permissions and can operate in sensitive Windows contexts (including Session 0/service workflows and Winlogon-adjacent components).

As a result, all code changes must:

- default to least privilege,
- provide explicit audit logging,
- avoid stealth/evasion behavior,
- include safe-fail paths when trust checks fail,
- avoid uncontrolled remote-code execution,
- undergo peer review before merge.

---

## Engineer-Only Protocol

WinAIAgent includes startup safety gating designed for engineering lab usage.

Expected controls include:

- debugger-presence checks,
- virtualization/sandbox heuristics (e.g., VMware/VirtualBox signatures),
- fail-fast behavior when environment safety criteria are not met.

These controls are part of the project’s defense-in-depth posture and must not be removed in normal development.

---

## Ethical Use Policy

Use of this code for offensive abuse is strictly prohibited.

Forbidden uses include (non-exhaustive):

- malware development,
- ransomware tooling,
- unauthorized surveillance,
- credential theft,
- unauthorized persistence/evasion,
- unauthorized privilege escalation on third-party systems.

Any contribution that enables abusive behavior will be rejected.

---

## Sandboxing Requirements

⚠️ **Run in isolated test environments only**:

- ephemeral VMs,
- controlled sandboxes,
- disposable snapshots.

Running on bare-metal personal devices or production infrastructure violates project intent and creates unacceptable risk.

---

## AI Alignment Disclaimer

This project studies how LLM-driven decision systems behave under system-level autonomy constraints.

Primary goals:

- evaluate alignment and safety boundaries,
- validate constrained execution strategies,
- improve explainability and rollback behavior for autonomous remediation.

WinAIAgent is not a production endpoint-management product and is not intended for unsupervised deployment.

---

## Security Review Expectations for Pull Requests

Security-relevant pull requests should include:

- threat model assumptions,
- privilege impact analysis,
- abuse-case analysis,
- mitigation strategy,
- validation evidence (logs/tests).

Maintainers may require additional hardening before approval.

