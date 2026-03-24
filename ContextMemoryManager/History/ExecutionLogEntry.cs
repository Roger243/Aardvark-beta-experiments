using System;
using System.Collections.Generic;
using System.Linq;
using CommandExecutorModule.Executors;
using ContextMemoryManager.Models;
using SystemMonitorModule.Models;

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

    public static ExecutionLogEntry Create(
        string commandText,
        PowerShellExecutionResult? executionResult,
        IReadOnlyList<SystemMetrics> snapshotBefore,
        IReadOnlyList<SystemMetrics> snapshotAfter,
        EntrySeverity severity = EntrySeverity.Info,
        IReadOnlyCollection<string>? tags = null,
        string? summary = null)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(commandText);

        return new ExecutionLogEntry
        {
            CommandText = commandText,
            ExecutionResult = executionResult,
            SnapshotBefore = SystemSnapshot.FromMetrics(snapshotBefore),
            SnapshotAfter = SystemSnapshot.FromMetrics(snapshotAfter),
            Severity = severity,
            Tags = NormalizeTags(tags),
            Summary = string.IsNullOrWhiteSpace(summary)
                ? BuildDefaultSummary(commandText, executionResult)
                : summary.Trim(),
            TimestampUtc = DateTimeOffset.UtcNow
        };
    }

    private static IReadOnlyCollection<string> NormalizeTags(IReadOnlyCollection<string>? tags)
    {
        if (tags is null || tags.Count == 0)
        {
            return Array.Empty<string>();
        }

        return tags.Where(tag => !string.IsNullOrWhiteSpace(tag))
                   .Select(tag => tag.Trim().StartsWith('#') ? tag.Trim() : $"#{tag.Trim()}")
                   .Distinct(StringComparer.OrdinalIgnoreCase)
                   .ToArray();
    }

    private static string BuildDefaultSummary(string commandText, PowerShellExecutionResult? result)
    {
        if (result is null)
        {
            return $"Command queued: {commandText}";
        }

        return result.ExitCode == 0
            ? $"Command succeeded: {commandText}"
            : $"Command failed (exit {result.ExitCode}): {commandText}";
    }
}
