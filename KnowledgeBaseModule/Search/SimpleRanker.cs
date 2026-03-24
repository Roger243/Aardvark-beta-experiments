using System;
using System.Collections.Generic;
using System.Linq;
using KnowledgeBaseModule.Models;

namespace KnowledgeBaseModule.Search;

public sealed class SimpleRanker
{
    public IReadOnlyList<SearchResult> Rank(SearchQuery query, IEnumerable<KnowledgeSnippet> snippets)
    {
        var now = DateTimeOffset.UtcNow;
        var q = query.QueryText?.Trim() ?? string.Empty;

        return snippets
            .Select(s =>
            {
                var relevance = ComputeRelevance(q, s);
                var ageDays = Math.Max(0, (now - s.UpdatedUtc).TotalDays);
                var recency = 1.0 / (1.0 + ageDays / 30.0);
                var final = (0.65 * relevance) + (0.20 * recency) + (0.15 * s.ConfidenceScore);

                return new SearchResult(s, relevance, recency, final);
            })
            .OrderByDescending(r => r.FinalScore)
            .Take(Math.Clamp(query.Limit, 1, 200))
            .ToArray();
    }

    private static double ComputeRelevance(string query, KnowledgeSnippet snippet)
    {
        if (string.IsNullOrWhiteSpace(query))
        {
            return 0.5;
        }

        double score = 0;
        if (snippet.Title.Contains(query, StringComparison.OrdinalIgnoreCase)) score += 0.5;
        if (snippet.Content.Contains(query, StringComparison.OrdinalIgnoreCase)) score += 0.35;
        if (snippet.Tags.Any(t => t.Contains(query, StringComparison.OrdinalIgnoreCase))) score += 0.15;

        return Math.Min(score, 1.0);
    }
}
