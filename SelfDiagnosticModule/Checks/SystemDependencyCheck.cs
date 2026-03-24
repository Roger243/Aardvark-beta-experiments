using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using SelfDiagnosticModule.Models;

namespace SelfDiagnosticModule.Checks;

public sealed class SystemDependencyCheck : IHealthCheck
{
    public string Name => nameof(SystemDependencyCheck);

    public Task<DiagnosticReport> RunAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var findings = new List<string>();
        var fixes = new List<string>();

        var powershell = Environment.ExpandEnvironmentVariables(@"%WINDIR%\System32\WindowsPowerShell\v1.0\powershell.exe");
        if (File.Exists(powershell))
        {
            findings.Add("PowerShell detected.");
        }
        else
        {
            findings.Add("PowerShell not found.");
            fixes.Add("Install/enable Windows PowerShell feature.");
        }

        var status = fixes.Count == 0 ? HealthStatus.Healthy : HealthStatus.Degraded;
        return Task.FromResult(new DiagnosticReport(DateTimeOffset.UtcNow, status, findings, fixes));
    }
}
