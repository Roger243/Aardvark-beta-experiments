using System;
using System.Diagnostics;
using System.IO;

namespace FileIntelligenceModule.Models;

public sealed record FileMetadata
{
    public string Path { get; init; } = string.Empty;
    public long SizeBytes { get; init; }
    public FileAttributes Attributes { get; init; }
    public DateTimeOffset CreatedUtc { get; init; }
    public DateTimeOffset LastWriteUtc { get; init; }
    public string? Version { get; init; }
    public string? Sha256 { get; init; }

    public static FileMetadata FromFileInfo(FileInfo info, string? sha256)
    {
        string? version = null;
        try
        {
            version = FileVersionInfo.GetVersionInfo(info.FullName).FileVersion;
        }
        catch
        {
            // non-PE files won't have version metadata.
        }

        return new FileMetadata
        {
            Path = info.FullName,
            SizeBytes = info.Exists ? info.Length : 0,
            Attributes = info.Attributes,
            CreatedUtc = info.CreationTimeUtc,
            LastWriteUtc = info.LastWriteTimeUtc,
            Version = version,
            Sha256 = sha256
        };
    }
}
