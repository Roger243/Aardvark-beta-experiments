using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using ContextMemoryManager.Models;

namespace ContextMemoryManager.Storage;

public interface IPersistentStorage
{
    Task SaveAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<ExecutionLogEntry>> QueryAsync(MemorySearchQuery query, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<ExecutionLogEntry>> LoadRecentAsync(int count, CancellationToken cancellationToken = default);
}
