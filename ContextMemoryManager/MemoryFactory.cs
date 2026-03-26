using ContextMemoryManager.Context;
using ContextMemoryManager.Search;
using ContextMemoryManager.Storage;

namespace ContextMemoryManager;

public static class MemoryFactory
{
    public static (SessionContext Session, ContextSummarizer Summarizer, MemorySearchEngine Search, IPersistentStorage Storage)
        CreateDefault()
    {
        var session = new SessionContext(maxEntries: 20);
        var summarizer = new ContextSummarizer(new Utils.TokenCounter());
        var search = new MemorySearchEngine();
        IPersistentStorage storage = new JsonStorageProvider();

        return (session, summarizer, search, storage);
    }
}
