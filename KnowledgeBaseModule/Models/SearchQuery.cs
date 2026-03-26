using System;
using System.Collections.Generic;

namespace KnowledgeBaseModule.Models;

public sealed record SearchQuery(
    string QueryText,
    IReadOnlyList<string>? Tags = null,
    string? Category = null,
    DateTimeOffset? CreatedAfterUtc = null,
    int Limit = 20);
