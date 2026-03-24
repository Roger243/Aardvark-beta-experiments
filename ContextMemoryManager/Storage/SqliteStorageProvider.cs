using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using ContextMemoryManager.Models;

namespace ContextMemoryManager.Storage;

public sealed class SqliteStorageProvider : IPersistentStorage
{
    public Task SaveAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default)
        => throw new NotImplementedException("SQLite provider wiring is intentionally deferred to integration phase.");

    public Task<IReadOnlyList<ExecutionLogEntry>> QueryAsync(MemorySearchQuery query, CancellationToken cancellationToken = default)
        => Task.FromResult<IReadOnlyList<ExecutionLogEntry>>(Array.Empty<ExecutionLogEntry>());

    public Task<IReadOnlyList<ExecutionLogEntry>> LoadRecentAsync(int count, CancellationToken cancellationToken = default)
        => Task.FromResult<IReadOnlyList<ExecutionLogEntry>>(Array.Empty<ExecutionLogEntry>());
}
