using System.Threading;
using System.Threading.Tasks;
using KnowledgeBaseModule.Storage;

namespace KnowledgeBaseModule.Management;

public sealed class SnippetMaintainer
{
    private readonly IKnowledgeStore _store;

    public SnippetMaintainer(IKnowledgeStore store)
    {
        _store = store;
    }

    public Task<int> ArchiveStaleAsync(int olderThanDays = 180, CancellationToken cancellationToken = default)
        => _store.ArchiveOlderThanAsync(olderThanDays, cancellationToken);
}
