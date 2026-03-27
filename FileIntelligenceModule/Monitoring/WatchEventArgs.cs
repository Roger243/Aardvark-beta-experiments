using System;
using System.IO;

namespace FileIntelligenceModule.Monitoring;

public sealed class WatchEventArgs : EventArgs
{
    public WatcherChangeTypes ChangeType { get; }
    public string Path { get; }
    public string? OldPath { get; }
    public DateTimeOffset TimestampUtc { get; } = DateTimeOffset.UtcNow;

    public WatchEventArgs(WatcherChangeTypes changeType, string path, string? oldPath = null)
    {
        ChangeType = changeType;
        Path = path;
        OldPath = oldPath;
    }
}
