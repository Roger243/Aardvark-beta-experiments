using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using Microsoft.Extensions.Logging;
using KnowledgeBaseModule.Indexing;
using KnowledgeBaseModule.Management;
using KnowledgeBaseModule.Models;
using KnowledgeBaseModule.Search;
using KnowledgeBaseModule.Storage;
using KnowledgeBaseModule.Utils;

namespace KnowledgeBaseModule;

public sealed class KnowledgeService : IKnowledgeManager
{
    private readonly IKnowledgeStore _store;
    private readonly DocumentParser _parser;
    private readonly MetadataExtractor _metadataExtractor;
    private readonly SimpleRanker _ranker;
    private readonly SnippetMaintainer _maintainer;
    private readonly ILogger<KnowledgeService> _logger;

    public KnowledgeService(
        IKnowledgeStore store,
        DocumentParser parser,
        MetadataExtractor metadataExtractor,
        SimpleRanker ranker,
        SnippetMaintainer maintainer,
        ILogger<KnowledgeService> logger)
    {
        _store = store;
        _parser = parser;
        _metadataExtractor = metadataExtractor;
        _ranker = ranker;
        _maintainer = maintainer;
        _logger = logger;
    }

    public async Task<long> AddSnippetAsync(KnowledgeSnippet snippet, CancellationToken cancellationToken = default)
    {
        var sanitized = snippet with
        {
            Title = StringSanitizer.Clean(snippet.Title),
            Content = StringSanitizer.Clean(snippet.Content),
            Tags = snippet.Tags.Select(StringSanitizer.Clean).Where(t => !string.IsNullOrWhiteSpace(t)).Distinct(StringComparer.OrdinalIgnoreCase).ToArray(),
            UpdatedUtc = DateTimeOffset.UtcNow,
            CreatedUtc = snippet.CreatedUtc == default ? DateTimeOffset.UtcNow : snippet.CreatedUtc
        };

        return await _store.InsertAsync(sanitized, cancellationToken).ConfigureAwait(false);
    }

    public async Task<IReadOnlyList<SearchResult>> SearchAsync(SearchQuery query, CancellationToken cancellationToken = default)
    {
        var raw = await _store.SearchAsync(query, cancellationToken).ConfigureAwait(false);
        return _ranker.Rank(query, raw);
    }

    public async Task<int> IndexPathAsync(string directoryOrFilePath, CancellationToken cancellationToken = default)
    {
        var parsed = await _parser.ParseAsync(directoryOrFilePath, cancellationToken).ConfigureAwait(false);
        var inserted = 0;

        foreach (var (path, content) in parsed)
        {
            cancellationToken.ThrowIfCancellationRequested();

            var snippet = new KnowledgeSnippet
            {
                Title = Path.GetFileName(path),
                Content = StringSanitizer.Clean(content),
                Tags = _metadataExtractor.ExtractTags(content),
                Category = "document",
                ConfidenceScore = 0.7,
                CreatedUtc = DateTimeOffset.UtcNow,
                UpdatedUtc = DateTimeOffset.UtcNow
            };

            await _store.InsertAsync(snippet, cancellationToken).ConfigureAwait(false);
            inserted++;
        }

        _logger.LogInformation("Indexed {Count} documents into knowledge store.", inserted);
        return inserted;
    }

    public async Task<bool> PromoteFromCommandHistoryAsync(ExecutionLogEntry entry, CancellationToken cancellationToken = default)
    {
        if (entry is null)
        {
            return false;
        }

        var title = $"Command: {entry.CommandText}";
        var exitCode = entry.ExecutionResult?.ExitCode ?? -1;
        var output = entry.ExecutionResult?.StandardOutput ?? string.Empty;
        var errors = entry.ExecutionResult?.StandardError ?? string.Empty;
        var content = $"Command: {entry.CommandText}\nExitCode: {exitCode}\nOutput:\n{output}\nErrors:\n{errors}";
        var snippet = new KnowledgeSnippet
        {
            Title = StringSanitizer.Clean(title),
            Content = StringSanitizer.Clean(content),
            Tags = new[] { "command", "history", "winagent" },
            Category = "command-history",
            ConfidenceScore = entry.Succeeded ? 0.9 : 0.4,
            CreatedUtc = DateTimeOffset.UtcNow,
            UpdatedUtc = DateTimeOffset.UtcNow
        };

        await _store.InsertAsync(snippet, cancellationToken).ConfigureAwait(false);
        await _maintainer.ArchiveStaleAsync(cancellationToken: cancellationToken).ConfigureAwait(false);
        return true;
    }
}
