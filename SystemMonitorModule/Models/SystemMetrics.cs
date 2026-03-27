using System;

namespace SystemMonitorModule.Models;

public sealed record SystemMetrics
{
    public string Source { get; init; } = string.Empty;
    public DateTimeOffset TimestampUtc { get; init; }

    public double CpuUsagePercent { get; init; }
    public double MemoryUsedMb { get; init; }
    public double MemoryTotalMb { get; init; }

    public double MemoryUsagePercent => MemoryTotalMb <= 0 ? 0 : (MemoryUsedMb / MemoryTotalMb) * 100;
}
