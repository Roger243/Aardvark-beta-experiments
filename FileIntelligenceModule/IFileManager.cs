using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using FileIntelligenceModule.Models;
using FileIntelligenceModule.Monitoring;
using FileIntelligenceModule.Search;

namespace FileIntelligenceModule;

public interface IFileManager : IAsyncDisposable
{
    Task<IReadOnlyList<FileMetadata>> SearchAsync(SearchCriteria criteria, CancellationToken cancellationToken = default);

    Task<FileMetadata> GetMetadataAsync(string path, CancellationToken cancellationToken = default);

    Task WriteAllTextAtomicAsync(string path, string content, CancellationToken cancellationToken = default);

    Task<string> ComputeSha256Async(string path, CancellationToken cancellationToken = default);

    Task<bool> CanModifyAsync(string path, CancellationToken cancellationToken = default);

    Task StartWatchingAsync(string path, CancellationToken cancellationToken = default);
    Task StopWatchingAsync(string path, CancellationToken cancellationToken = default);

    event EventHandler<WatchEventArgs>? FileChanged;
}
