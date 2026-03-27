using System;
using System.Collections.Generic;
using System.Linq;
using SystemMonitorModule.Models;

namespace ContextMemoryManager.Models;

public sealed record SystemSnapshot
{
    public DateTimeOffset TimestampUtc { get; init; } = DateTimeOffset.UtcNow;

    public double CpuUsagePercent { get; init; }
    public double MemoryUsagePercent { get; init; }

    public IReadOnlyList<string> ActiveProcesses { get; init; } = Array.Empty<string>();
    public IReadOnlyDictionary<string, string> RegistryValues { get; init; } = new Dictionary<string, string>();

    public static SystemSnapshot FromMetrics(IEnumerable<SystemMetrics> metrics)
    {
        var latest = metrics.OrderByDescending(m => m.TimestampUtc).FirstOrDefault();
        if (latest is null)
        {
            return new SystemSnapshot();
        }

        return new SystemSnapshot
        {
            TimestampUtc = latest.TimestampUtc,
            CpuUsagePercent = latest.CpuUsagePercent,
            MemoryUsagePercent = latest.MemoryUsagePercent
        };
    }
}
