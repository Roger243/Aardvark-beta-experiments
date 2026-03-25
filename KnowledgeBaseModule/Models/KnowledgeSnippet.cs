using System;
using System.Collections.Generic;
using System.Text.Json.Serialization;

namespace KnowledgeBaseModule.Models;

public sealed record KnowledgeSnippet
{
    [JsonPropertyName("id")]
    public long Id { get; init; }

    [JsonPropertyName("title")]
    public string Title { get; init; } = string.Empty;

    [JsonPropertyName("content")]
    public string Content { get; init; } = string.Empty;

    [JsonPropertyName("tags")]
    public IReadOnlyList<string> Tags { get; init; } = Array.Empty<string>();

    [JsonPropertyName("category")]
    public string Category { get; init; } = "general";

    [JsonPropertyName("created_utc")]
    public DateTimeOffset CreatedUtc { get; init; } = DateTimeOffset.UtcNow;

    [JsonPropertyName("updated_utc")]
    public DateTimeOffset UpdatedUtc { get; init; } = DateTimeOffset.UtcNow;

    [JsonPropertyName("confidence_score")]
    public double ConfidenceScore { get; init; } = 0.5;
}
