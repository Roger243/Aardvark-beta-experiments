using System;
using System.IO;

namespace FileIntelligenceModule.Monitoring;

public sealed class DirectoryWatcher : IDisposable
{
    private FileSystemWatcher? _watcher;

    public event EventHandler<WatchEventArgs>? FileChanged;

    public void Start(string path)
    {
        Stop();

        var target = Directory.Exists(path) ? path : Path.GetDirectoryName(path) ?? path;
        _watcher = new FileSystemWatcher(target)
        {
            IncludeSubdirectories = true,
            NotifyFilter = NotifyFilters.FileName | NotifyFilters.LastWrite | NotifyFilters.DirectoryName | NotifyFilters.Size,
            EnableRaisingEvents = true
        };

        _watcher.Created += (_, e) => Raise(e.ChangeType, e.FullPath);
        _watcher.Changed += (_, e) => Raise(e.ChangeType, e.FullPath);
        _watcher.Deleted += (_, e) => Raise(e.ChangeType, e.FullPath);
        _watcher.Renamed += (_, e) => FileChanged?.Invoke(this,
            new WatchEventArgs(WatcherChangeTypes.Renamed, e.FullPath, oldPath: e.OldFullPath));
    }

    public void Stop()
    {
        if (_watcher is null) return;

        _watcher.EnableRaisingEvents = false;
        _watcher.Dispose();
        _watcher = null;
    }

    public void Dispose() => Stop();

    private void Raise(WatcherChangeTypes type, string path)
    {
        FileChanged?.Invoke(this, new WatchEventArgs(type, path));
    }
}
