using System;
using System.Collections.Generic;

namespace ContextMemoryManager.Models;

public sealed record MemorySearchQuery
{
    public string? ContainsText { get; init; }
    public IReadOnlyCollection<string> RequiredTags { get; init; } = Array.Empty<string>();
    public DateTimeOffset? FromUtc { get; init; }
    public DateTimeOffset? ToUtc { get; init; }
    public int MaxResults { get; init; } = 50;
}
