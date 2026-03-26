using Microsoft.Extensions.Logging;
using KnowledgeBaseModule.Indexing;
using KnowledgeBaseModule.Management;
using KnowledgeBaseModule.Search;
using KnowledgeBaseModule.Storage;

namespace KnowledgeBaseModule;

public static class KnowledgeModuleFactory
{
    public static async System.Threading.Tasks.Task<IKnowledgeManager> CreateDefaultAsync(
        string sqlitePath,
        ILoggerFactory loggerFactory)
    {
        IKnowledgeStore store = new SqliteKnowledgeStore(sqlitePath);
        await store.InitializeAsync().ConfigureAwait(false);

        var parser = new DocumentParser();
        var extractor = new MetadataExtractor();
        var ranker = new SimpleRanker();
        var maintainer = new SnippetMaintainer(store);

        return new KnowledgeService(
            store,
            parser,
            extractor,
            ranker,
            maintainer,
            loggerFactory.CreateLogger<KnowledgeService>());
    }
}
