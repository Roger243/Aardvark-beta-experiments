using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace KnowledgeBaseModule.Indexing;

public sealed class MetadataExtractor
{
    private static readonly Regex WordRegex = new("[A-Za-z][A-Za-z0-9_\-]{3,}", RegexOptions.Compiled);

    public IReadOnlyList<string> ExtractTags(string content, int maxTags = 8)
    {
        var frequencies = new Dictionary<string, int>(StringComparer.OrdinalIgnoreCase);
        foreach (Match m in WordRegex.Matches(content))
        {
            var word = m.Value.ToLowerInvariant();
            frequencies[word] = frequencies.TryGetValue(word, out var c) ? c + 1 : 1;
        }

        return frequencies
            .OrderByDescending(kv => kv.Value)
            .ThenBy(kv => kv.Key)
            .Take(maxTags)
            .Select(kv => kv.Key)
            .ToArray();
    }
}
