using System;
using System.Threading;
using System.Threading.Tasks;
using SystemMonitorModule.Events;
using SystemMonitorModule.Models;

namespace SystemMonitorModule.Providers;

public interface IMonitorProvider : IAsyncDisposable
{
    string Name { get; }

    event EventHandler<MonitorEventArgs>? ProviderError;

    Task StartAsync(CancellationToken cancellationToken = default);
    Task StopAsync(CancellationToken cancellationToken = default);
    Task<SystemMetrics> CollectAsync(CancellationToken cancellationToken = default);
}
