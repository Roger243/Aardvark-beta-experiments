namespace SystemMonitorModule.Models;

public sealed record MonitorThresholds
{
    public double MaxCpuUsagePercent { get; init; } = 90.0;
    public double MaxMemoryUsagePercent { get; init; } = 90.0;

    public bool IsExceeded(SystemMetrics metric, out string reason)
    {
        if (metric.CpuUsagePercent > MaxCpuUsagePercent)
        {
            reason = $"CPU usage {metric.CpuUsagePercent:F2}% exceeded threshold {MaxCpuUsagePercent:F2}%";
            return true;
        }

        if (metric.MemoryUsagePercent > MaxMemoryUsagePercent)
        {
            reason = $"Memory usage {metric.MemoryUsagePercent:F2}% exceeded threshold {MaxMemoryUsagePercent:F2}%";
            return true;
        }

        reason = string.Empty;
        return false;
    }
}
