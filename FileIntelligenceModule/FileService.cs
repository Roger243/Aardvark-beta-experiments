using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using FileIntelligenceModule.Exceptions;
using FileIntelligenceModule.Models;
using FileIntelligenceModule.Monitoring;
using FileIntelligenceModule.Operations;
using FileIntelligenceModule.Search;
using FileIntelligenceModule.Security;

namespace FileIntelligenceModule;

public sealed class FileService : IFileManager
{
    private readonly FileSearcher _searcher;
    private readonly AtomicWriter _atomicWriter;
    private readonly AccessValidator _accessValidator;
    private readonly FileHasher _fileHasher;
    private readonly ILogger<FileService> _logger;
    private readonly ConcurrentDictionary<string, DirectoryWatcher> _watchers = new(StringComparer.OrdinalIgnoreCase);

    public event EventHandler<WatchEventArgs>? FileChanged;

    public FileService(
        FileSearcher searcher,
        AtomicWriter atomicWriter,
        AccessValidator accessValidator,
        FileHasher fileHasher,
        ILogger<FileService> logger)
    {
        _searcher = searcher;
        _atomicWriter = atomicWriter;
        _accessValidator = accessValidator;
        _fileHasher = fileHasher;
        _logger = logger;
    }

    public Task<IReadOnlyList<FileMetadata>> SearchAsync(SearchCriteria criteria, CancellationToken cancellationToken = default)
        => _searcher.SearchAsync(criteria, cancellationToken);

    public async Task<FileMetadata> GetMetadataAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var normalized = PathUtil.NormalizeLongPath(path);
        var info = new FileInfo(normalized);
        if (!info.Exists)
        {
            throw new FileIntelligenceException($"File not found: {path}");
        }

        var sha = await _fileHasher.ComputeSha256Async(normalized, cancellationToken).ConfigureAwait(false);

        return FileMetadata.FromFileInfo(info, sha);
    }

    public async Task WriteAllTextAtomicAsync(string path, string content, CancellationToken cancellationToken = default)
    {
        if (!await CanModifyAsync(path, cancellationToken).ConfigureAwait(false))
        {
            throw new FileIntelligenceException($"Modification denied by access validator: {path}");
        }

        await _atomicWriter.WriteAllTextAsync(path, content, cancellationToken).ConfigureAwait(false);
    }

    public Task<string> ComputeSha256Async(string path, CancellationToken cancellationToken = default)
        => _fileHasher.ComputeSha256Async(path, cancellationToken);

    public async Task<bool> CanModifyAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        return await _accessValidator.CanWriteAsync(path, cancellationToken).ConfigureAwait(false);
    }

    public Task StartWatchingAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var normalized = PathUtil.NormalizeLongPath(path);
        var watcher = _watchers.GetOrAdd(normalized, static _ => new DirectoryWatcher());
        watcher.FileChanged += OnWatcherChanged;
        watcher.Start(normalized);

        _logger.LogInformation("Started file watch on {Path}", path);
        return Task.CompletedTask;
    }

    public Task StopWatchingAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var normalized = PathUtil.NormalizeLongPath(path);
        if (_watchers.TryRemove(normalized, out var watcher))
        {
            watcher.FileChanged -= OnWatcherChanged;
            watcher.Stop();
            watcher.Dispose();

            _logger.LogInformation("Stopped file watch on {Path}", path);
        }

        return Task.CompletedTask;
    }

    public async ValueTask DisposeAsync()
    {
        foreach (var kv in _watchers)
        {
            kv.Value.FileChanged -= OnWatcherChanged;
            kv.Value.Stop();
            kv.Value.Dispose();
        }

        _watchers.Clear();
        await Task.CompletedTask;
    }

    private void OnWatcherChanged(object? sender, WatchEventArgs e)
    {
        FileChanged?.Invoke(this, e);
    }
}
