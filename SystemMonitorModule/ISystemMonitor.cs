using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using SystemMonitorModule.Events;
using SystemMonitorModule.Models;

namespace SystemMonitorModule;

public interface ISystemMonitor : IAsyncDisposable
{
    event EventHandler<MonitorEventArgs>? ThresholdExceeded;
    event EventHandler<MonitorEventArgs>? ProviderError;

    bool IsRunning { get; }

    Task StartAsync(CancellationToken cancellationToken = default);
    Task StopAsync(CancellationToken cancellationToken = default);

    Task<IReadOnlyList<SystemMetrics>> CollectSnapshotAsync(CancellationToken cancellationToken = default);
    Task UpdateThresholdsAsync(MonitorThresholds thresholds, CancellationToken cancellationToken = default);
}
