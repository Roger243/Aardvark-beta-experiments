using System;
using SystemMonitorModule.Models;

namespace SystemMonitorModule.Events;

public sealed class MonitorEventArgs : EventArgs
{
    public string ProviderName { get; init; } = string.Empty;
    public string Message { get; init; } = string.Empty;
    public Exception? Exception { get; init; }
    public SystemMetrics? Metrics { get; init; }
    public LogEntry? LogEntry { get; init; }

    public static MonitorEventArgs FromMetric(SystemMetrics metric, string message) => new()
    {
        ProviderName = metric.Source,
        Metrics = metric,
        Message = message
    };

    public static MonitorEventArgs FromError(string providerName, Exception exception) => new()
    {
        ProviderName = providerName,
        Exception = exception,
        Message = exception.Message
    };

    public static MonitorEventArgs FromLog(LogEntry log) => new()
    {
        ProviderName = log.Provider,
        LogEntry = log,
        Message = log.Message
    };
}
