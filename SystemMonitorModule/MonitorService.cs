using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SystemMonitorModule.Events;
using SystemMonitorModule.Models;
using SystemMonitorModule.Providers;

namespace SystemMonitorModule;

public sealed class MonitorService : ISystemMonitor
{
    private readonly IReadOnlyList<IMonitorProvider> _providers;
    private readonly ILogger<MonitorService> _logger;
    private MonitorThresholds _thresholds;

    public event EventHandler<MonitorEventArgs>? ThresholdExceeded;
    public event EventHandler<MonitorEventArgs>? ProviderError;

    public bool IsRunning { get; private set; }

    public MonitorService(IEnumerable<IMonitorProvider> providers, MonitorThresholds thresholds, ILogger<MonitorService> logger)
    {
        _providers = providers.ToList();
        _thresholds = thresholds;
        _logger = logger;

        foreach (var provider in _providers)
        {
            provider.ProviderError += (_, e) => ProviderError?.Invoke(this, e);
        }
    }

    public async Task StartAsync(CancellationToken cancellationToken = default)
    {
        if (IsRunning) return;

        foreach (var provider in _providers)
        {
            await provider.StartAsync(cancellationToken).ConfigureAwait(false);
        }

        IsRunning = true;
        _logger.LogInformation("System monitor started with {Count} providers", _providers.Count);
    }

    public async Task StopAsync(CancellationToken cancellationToken = default)
    {
        if (!IsRunning) return;

        foreach (var provider in _providers)
        {
            await provider.StopAsync(cancellationToken).ConfigureAwait(false);
        }

        IsRunning = false;
        _logger.LogInformation("System monitor stopped");
    }

    public async Task<IReadOnlyList<SystemMetrics>> CollectSnapshotAsync(CancellationToken cancellationToken = default)
    {
        var snapshots = new List<SystemMetrics>(_providers.Count);

        foreach (var provider in _providers)
        {
            var metric = await provider.CollectAsync(cancellationToken).ConfigureAwait(false);
            snapshots.Add(metric);

            if (_thresholds.IsExceeded(metric, out var reason))
            {
                ThresholdExceeded?.Invoke(this, MonitorEventArgs.FromMetric(metric, reason));
            }
        }

        return snapshots;
    }

    public Task UpdateThresholdsAsync(MonitorThresholds thresholds, CancellationToken cancellationToken = default)
    {
        _thresholds = thresholds;
        _logger.LogInformation("Thresholds updated");
        return Task.CompletedTask;
    }

    public async ValueTask DisposeAsync()
    {
        await StopAsync().ConfigureAwait(false);
        foreach (var provider in _providers)
        {
            await provider.DisposeAsync().ConfigureAwait(false);
        }
    }
}
