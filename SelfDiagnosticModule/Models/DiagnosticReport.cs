using System;
using System.Collections.Generic;

namespace SelfDiagnosticModule.Models;

public sealed record DiagnosticReport(
    DateTimeOffset GeneratedAtUtc,
    HealthStatus Status,
    IReadOnlyList<string> Findings,
    IReadOnlyList<string> SuggestedFixes);
