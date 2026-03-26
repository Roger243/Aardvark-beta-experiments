using System;
using System.Collections.Generic;
using System.Linq;
using ContextMemoryManager.History;
using ContextMemoryManager.Models;

namespace ContextMemoryManager.Search;

public sealed class MemorySearchEngine
{
    public IReadOnlyList<ExecutionLogEntry> Search(IReadOnlyList<ExecutionLogEntry> source, MemorySearchQuery query)
    {
        var q = source.AsEnumerable();

        if (!string.IsNullOrWhiteSpace(query.ContainsText))
        {
            q = q.Where(e => e.CommandText.Contains(query.ContainsText, StringComparison.OrdinalIgnoreCase)
                             || e.Summary.Contains(query.ContainsText, StringComparison.OrdinalIgnoreCase));
        }

        if (query.RequiredTags.Count > 0)
        {
            q = q.Where(e => query.RequiredTags.All(tag => e.Tags.Contains(tag, StringComparer.OrdinalIgnoreCase)));
        }

        if (query.FromUtc.HasValue)
        {
            q = q.Where(e => e.TimestampUtc >= query.FromUtc.Value);
        }

        if (query.ToUtc.HasValue)
        {
            q = q.Where(e => e.TimestampUtc <= query.ToUtc.Value);
        }

        return q.OrderByDescending(e => e.TimestampUtc)
                .Take(Math.Max(1, query.MaxResults))
                .ToList();
    }
}
