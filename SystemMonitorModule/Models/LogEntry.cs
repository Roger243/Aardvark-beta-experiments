using System;

namespace SystemMonitorModule.Models;

public sealed record LogEntry
{
    public DateTimeOffset TimestampUtc { get; init; }
    public string Channel { get; init; } = string.Empty;
    public int EventId { get; init; }
    public string Level { get; init; } = string.Empty;
    public string Provider { get; init; } = string.Empty;
    public string MachineName { get; init; } = string.Empty;
    public string Message { get; init; } = string.Empty;
}
