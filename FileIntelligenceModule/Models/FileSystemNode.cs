using System.Collections.Generic;

namespace FileIntelligenceModule.Models;

public sealed record FileSystemNode
{
    public string Name { get; init; } = string.Empty;
    public string FullPath { get; init; } = string.Empty;
    public bool IsDirectory { get; init; }

    public IReadOnlyList<FileSystemNode> Children { get; init; } = new List<FileSystemNode>();
}
