using KnowledgeBaseModule.Models;

namespace KnowledgeBaseModule.Search;

public sealed record SearchResult(
    KnowledgeSnippet Snippet,
    double RelevanceScore,
    double RecencyScore,
    double FinalScore);
