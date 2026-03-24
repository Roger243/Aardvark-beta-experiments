using System;
using System.Management;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SystemMonitorModule.Events;
using SystemMonitorModule.Exceptions;
using SystemMonitorModule.Models;
using SystemMonitorModule.Wmi;

namespace SystemMonitorModule.Providers;

public sealed class WmiProvider : IMonitorProvider
{
    private readonly ILogger<WmiProvider> _logger;

    public string Name => "WMI";

    public event EventHandler<MonitorEventArgs>? ProviderError;

    public WmiProvider(ILogger<WmiProvider> logger)
    {
        _logger = logger;
    }

    public Task StartAsync(CancellationToken cancellationToken = default)
    {
        _logger.LogInformation("{Provider} started", Name);
        return Task.CompletedTask;
    }

    public Task StopAsync(CancellationToken cancellationToken = default)
    {
        _logger.LogInformation("{Provider} stopped", Name);
        return Task.CompletedTask;
    }

    public Task<SystemMetrics> CollectAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            cancellationToken.ThrowIfCancellationRequested();

            var cpuPercent = ReadCpuUsagePercent();
            var (memoryUsedMb, memoryTotalMb) = ReadMemoryUsage();

            return Task.FromResult(new SystemMetrics
            {
                Source = Name,
                TimestampUtc = DateTimeOffset.UtcNow,
                CpuUsagePercent = cpuPercent,
                MemoryUsedMb = memoryUsedMb,
                MemoryTotalMb = memoryTotalMb
            });
        }
        catch (Exception ex)
        {
            var wrapped = new MonitorException($"{Name} provider failed to collect metrics.", ex);
            _logger.LogError(wrapped, "{Provider} collection error", Name);
            ProviderError?.Invoke(this, MonitorEventArgs.FromError(Name, wrapped));
            throw wrapped;
        }
    }

    public ValueTask DisposeAsync() => ValueTask.CompletedTask;

    private static double ReadCpuUsagePercent()
    {
        var query = WmiQueryBuilder.SelectFrom("Win32_Processor", "LoadPercentage");
        using var searcher = new ManagementObjectSearcher(query);
        using var results = searcher.Get();

        double total = 0;
        var count = 0;

        foreach (ManagementObject cpu in results)
        {
            if (cpu["LoadPercentage"] is null) continue;
            total += Convert.ToDouble(cpu["LoadPercentage"]);
            count++;
        }

        return count == 0 ? 0 : total / count;
    }

    private static (double usedMb, double totalMb) ReadMemoryUsage()
    {
        var query = WmiQueryBuilder.SelectFrom("Win32_OperatingSystem", "TotalVisibleMemorySize", "FreePhysicalMemory");
        using var searcher = new ManagementObjectSearcher(query);
        using var results = searcher.Get();

        foreach (ManagementObject os in results)
        {
            var totalKb = Convert.ToDouble(os["TotalVisibleMemorySize"] ?? 0);
            var freeKb = Convert.ToDouble(os["FreePhysicalMemory"] ?? 0);
            var usedKb = Math.Max(0, totalKb - freeKb);

            return (usedKb / 1024d, totalKb / 1024d);
        }

        return (0, 0);
    }
}
