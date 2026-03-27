using System;
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

    /// <summary>
    /// Summarization strategy:
    /// 1) Always prioritize critical entries.
    /// 2) Backfill with most-recent entries.
    /// 3) Stop when estimated token budget is exceeded.
    /// </summary>
    public string BuildSummary(IReadOnlyList<ExecutionLogEntry> entries, int maxEstimatedTokens)
    {
        if (entries.Count == 0 || maxEstimatedTokens <= 0)
        {
            return string.Empty;
        }

        var critical = entries.Where(e => e.Severity == EntrySeverity.Critical)
                              .OrderByDescending(e => e.TimestampUtc)
                              .ToList();

        var recent = entries.OrderByDescending(e => e.TimestampUtc).ToList();

        var selected = new List<ExecutionLogEntry>();
        var seen = new HashSet<Guid>();

        foreach (var c in critical)
        {
            if (seen.Add(c.Id)) selected.Add(c);
        }

        foreach (var r in recent)
        {
            if (seen.Add(r.Id)) selected.Add(r);
        }

        var ordered = selected.OrderBy(e => e.TimestampUtc).ToList();

        var builder = new StringBuilder();
        var includedCount = 0;

        foreach (var entry in ordered)
        {
            var line = FormatEntry(entry);
            var candidate = builder.Length == 0 ? line : builder + Environment.NewLine + line;
            if (_tokenCounter.EstimateTokens(candidate.ToString()) > maxEstimatedTokens)
            {
                break;
            }

            if (builder.Length > 0)
            {
                builder.AppendLine();
            }

            builder.Append(line);
            includedCount++;
        }

        var truncatedCount = entries.Count - includedCount;
        if (truncatedCount > 0)
        {
            var tail = $"... ({truncatedCount} older entries omitted for token budget)";
            var candidate = builder.Length == 0 ? tail : builder + Environment.NewLine + tail;
            if (_tokenCounter.EstimateTokens(candidate.ToString()) <= maxEstimatedTokens)
            {
                if (builder.Length > 0)
                {
                    builder.AppendLine();
                }
                builder.Append(tail);
            }
        }

        return builder.ToString();
    }

    private static string FormatEntry(ExecutionLogEntry entry)
    {
        var tags = entry.Tags.Count == 0 ? string.Empty : $" tags=[{string.Join(',', entry.Tags)}]";
        var status = entry.Succeeded ? "ok" : "failed";
        return $"[{entry.TimestampUtc:u}] ({entry.Severity}) {status}: {entry.CommandText}{tags} :: {entry.Summary}";
    }
}
