using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using CommandExecutorModule.Executors;
using ContextMemoryManager.History;
using ContextMemoryManager.Models;
using SystemMonitorModule.Models;

namespace ContextMemoryManager;

public interface IMemoryManager : IAsyncDisposable
{
    /// <summary>
    /// Current volatile short-term memory for the active session.
    /// </summary>
    SessionContext CurrentSession { get; }

    /// <summary>
    /// Records a pre-constructed execution entry into session + persistent memory.
    /// </summary>
    Task RecordExecutionAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default);

    /// <summary>
    /// Helper for cross-module integration: creates and records an entry from executor + monitor outputs.
    /// </summary>
    Task RecordExecutionAsync(
        string commandText,
        PowerShellExecutionResult? executionResult,
        IReadOnlyList<SystemMetrics> snapshotBefore,
        IReadOnlyList<SystemMetrics> snapshotAfter,
        EntrySeverity severity = EntrySeverity.Info,
        IReadOnlyCollection<string>? tags = null,
        string? summary = null,
        CancellationToken cancellationToken = default);

    Task<IReadOnlyList<ExecutionLogEntry>> GetRecentHistoryAsync(int count, CancellationToken cancellationToken = default);

    Task<IReadOnlyList<ExecutionLogEntry>> SearchAsync(MemorySearchQuery query, CancellationToken cancellationToken = default);

    /// <summary>
    /// Builds a context payload suitable for LLM prompt usage under a token budget.
    /// </summary>
    Task<string> BuildPromptContextAsync(int maxEstimatedTokens, CancellationToken cancellationToken = default);
}
