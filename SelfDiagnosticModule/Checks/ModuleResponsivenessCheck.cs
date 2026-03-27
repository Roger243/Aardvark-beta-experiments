using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SelfDiagnosticModule.Models;

namespace SelfDiagnosticModule.Checks;

public sealed class ModuleResponsivenessCheck : IHealthCheck
{
    private readonly IReadOnlyDictionary<string, Func<CancellationToken, Task<bool>>> _probes;
    private readonly ILogger<ModuleResponsivenessCheck> _logger;

    public ModuleResponsivenessCheck(
        IReadOnlyDictionary<string, Func<CancellationToken, Task<bool>>> probes,
        ILogger<ModuleResponsivenessCheck> logger)
    {
        _probes = probes;
        _logger = logger;
    }

    public string Name => nameof(ModuleResponsivenessCheck);

    public async Task<DiagnosticReport> RunAsync(CancellationToken cancellationToken = default)
    {
        var findings = new List<string>();
        var fixes = new List<string>();
        var unhealthyCount = 0;

        foreach (var probe in _probes)
        {
            try
            {
                var ok = await probe.Value(cancellationToken).ConfigureAwait(false);
                if (ok)
                {
                    findings.Add($"Module responsive: {probe.Key}");
                }
                else
                {
                    unhealthyCount++;
                    findings.Add($"Module non-responsive: {probe.Key}");
                    fixes.Add($"Restart module '{probe.Key}'.");
                }
            }
            catch (Exception ex)
            {
                unhealthyCount++;
                findings.Add($"Probe error for '{probe.Key}': {ex.Message}");
                fixes.Add($"Inspect logs and restart '{probe.Key}'.");
                _logger.LogWarning(ex, "Module probe failed: {Module}", probe.Key);
            }
        }

        var status = unhealthyCount == 0 ? HealthStatus.Healthy : HealthStatus.Degraded;
        return new DiagnosticReport(DateTimeOffset.UtcNow, status, findings, fixes);
    }
}
