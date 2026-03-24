using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SystemMonitorModule.Events;
using SystemMonitorModule.Models;

namespace SystemMonitorModule.Providers;

public sealed class PerformanceCounterProvider : IMonitorProvider
{
    private readonly ILogger<PerformanceCounterProvider> _logger;

    public string Name => "PerformanceCounter";
    public event EventHandler<MonitorEventArgs>? ProviderError;

    public PerformanceCounterProvider(ILogger<PerformanceCounterProvider> logger) => _logger = logger;

    public Task StartAsync(CancellationToken cancellationToken = default) => Task.CompletedTask;
    public Task StopAsync(CancellationToken cancellationToken = default) => Task.CompletedTask;

    public Task<SystemMetrics> CollectAsync(CancellationToken cancellationToken = default)
    {
        return Task.FromResult(new SystemMetrics { Source = Name, TimestampUtc = DateTimeOffset.UtcNow });
    }

    public ValueTask DisposeAsync() => ValueTask.CompletedTask;
}
