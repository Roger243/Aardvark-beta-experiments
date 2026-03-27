using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using KnowledgeBaseModule.Models;

namespace KnowledgeBaseModule.Storage;

public interface IKnowledgeStore
{
    Task InitializeAsync(CancellationToken cancellationToken = default);
    Task<long> InsertAsync(KnowledgeSnippet snippet, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<KnowledgeSnippet>> SearchAsync(SearchQuery query, CancellationToken cancellationToken = default);
    Task<int> ArchiveOlderThanAsync(int days, CancellationToken cancellationToken = default);
}
