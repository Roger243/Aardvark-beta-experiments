using System.Collections.Generic;
using System.Linq;
using System.Text;
using ContextMemoryManager.History;
using ContextMemoryManager.Utils;

namespace ContextMemoryManager.Context;

public sealed class ContextSummarizer
{
    private readonly TokenCounter _tokenCounter;

    public ContextSummarizer(TokenCounter tokenCounter)
    {
        _tokenCounter = tokenCounter;
    }

    public string BuildSummary(IReadOnlyList<ExecutionLogEntry> entries, int maxEstimatedTokens)
    {
        if (entries.Count == 0 || maxEstimatedTokens <= 0)
        {
            return string.Empty;
        }

        // Keep all critical items and then backfill with recent items.
        var critical = entries.Where(e => e.Severity == EntrySeverity.Critical).ToList();
        var recent = entries.OrderByDescending(e => e.TimestampUtc).ToList();

        var chosen = new List<ExecutionLogEntry>(critical);
        var seen = new HashSet<System.Guid>(critical.Select(c => c.Id));

        foreach (var item in recent)
        {
            if (seen.Add(item.Id))
            {
                chosen.Add(item);
            }
        }

        // Render in chronological order and trim to token budget.
        var ordered = chosen.OrderBy(e => e.TimestampUtc).ToList();
        var builder = new StringBuilder();

        foreach (var entry in ordered)
        {
            var line = FormatEntry(entry);
            var projected = builder.Length == 0 ? line : builder + "\n" + line;

            if (_tokenCounter.EstimateTokens(projected.ToString()) > maxEstimatedTokens)
            {
                break;
            }

            if (builder.Length > 0)
            {
                builder.AppendLine();
            }
            builder.Append(line);
        }

        return builder.ToString();
    }

    private static string FormatEntry(ExecutionLogEntry entry)
    {
        var tags = entry.Tags.Count == 0 ? "" : $" tags=[{string.Join(',', entry.Tags)}]";
        var status = entry.Succeeded ? "ok" : "failed";
        return $"[{entry.TimestampUtc:u}] ({entry.Severity}) {status}: {entry.CommandText}{tags} :: {entry.Summary}";
    }
}
