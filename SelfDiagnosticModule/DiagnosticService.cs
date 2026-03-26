using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SelfDiagnosticModule.Checks;
using SelfDiagnosticModule.Healing;
using SelfDiagnosticModule.Models;

namespace SelfDiagnosticModule;

public sealed class DiagnosticService : IHealthManager, IAsyncDisposable
{
    private readonly IReadOnlyList<IHealthCheck> _checks;
    private readonly IReadOnlyList<IHealerAction> _healers;
    private readonly ILogger<DiagnosticService> _logger;
    private readonly TimeSpan _heartbeatInterval;

    private CancellationTokenSource? _heartbeatCts;
    private Task? _heartbeatTask;

    public DiagnosticService(
        IEnumerable<IHealthCheck> checks,
        IEnumerable<IHealerAction> healers,
        ILogger<DiagnosticService> logger,
        TimeSpan? heartbeatInterval = null)
    {
        _checks = checks.ToArray();
        _healers = healers.ToArray();
        _logger = logger;
        _heartbeatInterval = heartbeatInterval ?? TimeSpan.FromMinutes(1);
    }

    public Task<DiagnosticReport> RunQuickDiagnosticAsync(CancellationToken cancellationToken = default)
        => RunChecksAsync(_checks.Take(1), attemptHealing: false, cancellationToken);

    public Task<DiagnosticReport> RunFullDiagnosticAsync(CancellationToken cancellationToken = default)
        => RunChecksAsync(_checks, attemptHealing: true, cancellationToken);

    public Task StartHeartbeatAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (_heartbeatTask is { IsCompleted: false })
        {
            return Task.CompletedTask;
        }

        _heartbeatCts = new CancellationTokenSource();
        _heartbeatTask = Task.Run(() => HeartbeatLoopAsync(_heartbeatCts.Token), CancellationToken.None);

        _logger.LogInformation("Self-diagnostic heartbeat started.");
        return Task.CompletedTask;
    }

    public async Task StopHeartbeatAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (_heartbeatCts is null || _heartbeatTask is null)
        {
            return;
        }

        _heartbeatCts.Cancel();

        try
        {
            await _heartbeatTask.ConfigureAwait(false);
        }
        catch (OperationCanceledException)
        {
            // expected
        }
        finally
        {
            _heartbeatCts.Dispose();
            _heartbeatCts = null;
            _heartbeatTask = null;
        }

        _logger.LogInformation("Self-diagnostic heartbeat stopped.");
    }

    public async ValueTask DisposeAsync()
    {
        await StopHeartbeatAsync().ConfigureAwait(false);
    }

    private async Task HeartbeatLoopAsync(CancellationToken cancellationToken)
    {
        using var timer = new PeriodicTimer(_heartbeatInterval);

        while (!cancellationToken.IsCancellationRequested)
        {
            try
            {
                var report = await RunQuickDiagnosticAsync(cancellationToken).ConfigureAwait(false);
                _logger.LogDebug("Heartbeat status: {Status}", report.Status);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Heartbeat diagnostic iteration failed.");
            }

            try
            {
                await timer.WaitForNextTickAsync(cancellationToken).ConfigureAwait(false);
            }
            catch (OperationCanceledException)
            {
                break;
            }
        }
    }

    private async Task<DiagnosticReport> RunChecksAsync(
        IEnumerable<IHealthCheck> checks,
        bool attemptHealing,
        CancellationToken cancellationToken)
    {
        var findings = new List<string>();
        var fixes = new List<string>();
        var status = HealthStatus.Healthy;

        foreach (var check in checks)
        {
            cancellationToken.ThrowIfCancellationRequested();

            DiagnosticReport report;
            try
            {
                report = await check.RunAsync(cancellationToken).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Health check '{CheckName}' failed unexpectedly.", check.Name);
                findings.Add($"Health check crashed: {check.Name}");
                fixes.Add("Inspect diagnostic logs for details.");
                status = HealthStatus.Degraded;
                continue;
            }

            findings.AddRange(report.Findings.Select(f => $"[{check.Name}] {f}"));
            fixes.AddRange(report.SuggestedFixes);
            status = Max(status, report.Status);
        }

        if (attemptHealing && status != HealthStatus.Healthy)
        {
            await TryHealingAsync(findings, fixes, cancellationToken).ConfigureAwait(false);
        }

        return new DiagnosticReport(DateTimeOffset.UtcNow, status, findings, fixes);
    }

    private async Task TryHealingAsync(List<string> findings, List<string> fixes, CancellationToken cancellationToken)
    {
        foreach (var healer in _healers)
        {
            try
            {
                var ok = await healer.TryHealAsync(cancellationToken).ConfigureAwait(false);
                findings.Add(ok ? $"Auto-heal succeeded: {healer.Name}" : $"Auto-heal failed: {healer.Name}");
                if (!ok)
                {
                    fixes.Add($"Manual intervention may be required for healer '{healer.Name}'.");
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Healer action '{HealerName}' threw an exception.", healer.Name);
                findings.Add($"Auto-heal exception: {healer.Name}");
                fixes.Add($"Review healer logs for '{healer.Name}'.");
            }
        }
    }

    private static HealthStatus Max(HealthStatus left, HealthStatus right)
        => (HealthStatus)Math.Max((int)left, (int)right);
}
