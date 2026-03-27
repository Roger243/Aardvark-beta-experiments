using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using KnowledgeBaseModule.Models;
using KnowledgeBaseModule.Search;

namespace KnowledgeBaseModule;

public interface IKnowledgeManager
{
    Task<long> AddSnippetAsync(KnowledgeSnippet snippet, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<SearchResult>> SearchAsync(SearchQuery query, CancellationToken cancellationToken = default);
    Task<int> IndexPathAsync(string directoryOrFilePath, CancellationToken cancellationToken = default);
    Task<bool> PromoteFromCommandHistoryAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default);
}
