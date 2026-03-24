using System;
using System.Collections.Generic;
using CommandExecutorModule.Executors;
using ContextMemoryManager.Models;

namespace ContextMemoryManager.History;

public enum EntrySeverity
{
    Info = 0,
    Warning = 1,
    Critical = 2
}

public sealed record ExecutionLogEntry
{
    public Guid Id { get; init; } = Guid.NewGuid();
    public DateTimeOffset TimestampUtc { get; init; } = DateTimeOffset.UtcNow;

    public string CommandText { get; init; } = string.Empty;
    public PowerShellExecutionResult? ExecutionResult { get; init; }

    public SystemSnapshot? SnapshotBefore { get; init; }
    public SystemSnapshot? SnapshotAfter { get; init; }

    public EntrySeverity Severity { get; init; } = EntrySeverity.Info;
    public IReadOnlyCollection<string> Tags { get; init; } = Array.Empty<string>();

    public string Summary { get; init; } = string.Empty;

    public bool Succeeded => ExecutionResult is not null && ExecutionResult.ExitCode == 0;
}
