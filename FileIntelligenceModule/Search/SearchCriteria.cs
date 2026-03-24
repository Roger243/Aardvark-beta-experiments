using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace FileIntelligenceModule.Search;

public sealed record SearchCriteria
{
    public string RootPath { get; init; } = string.Empty;
    public bool Recursive { get; init; } = true;

    public IReadOnlyCollection<string> Wildcards { get; init; } = new[] { "*" };
    public IReadOnlyCollection<string> Extensions { get; init; } = Array.Empty<string>();
    public Regex? ContentPattern { get; init; }

    public int MaxResults { get; init; } = 500;
    public bool IncludeHiddenFiles { get; init; } = false;
}
