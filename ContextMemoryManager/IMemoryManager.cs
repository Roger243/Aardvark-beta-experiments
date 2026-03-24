using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using ContextMemoryManager.Models;

namespace ContextMemoryManager;

public interface IMemoryManager : IAsyncDisposable
{
    Task RecordExecutionAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default);

    Task<IReadOnlyList<ExecutionLogEntry>> GetRecentHistoryAsync(int count, CancellationToken cancellationToken = default);

    Task<IReadOnlyList<ExecutionLogEntry>> SearchAsync(MemorySearchQuery query, CancellationToken cancellationToken = default);

    Task<string> BuildPromptContextAsync(int maxEstimatedTokens, CancellationToken cancellationToken = default);

    SessionContext CurrentSession { get; }
}
