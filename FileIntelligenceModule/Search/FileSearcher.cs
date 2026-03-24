using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using FileIntelligenceModule.Exceptions;
using FileIntelligenceModule.Models;
using FileIntelligenceModule.Operations;

namespace FileIntelligenceModule.Search;

public sealed class FileSearcher
{
    private readonly FileHasher _fileHasher;
    private readonly ILogger<FileSearcher> _logger;

    public FileSearcher(FileHasher fileHasher, ILogger<FileSearcher> logger)
    {
        _fileHasher = fileHasher;
        _logger = logger;
    }

    public async Task<IReadOnlyList<FileMetadata>> SearchAsync(SearchCriteria criteria, CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(criteria);
        if (string.IsNullOrWhiteSpace(criteria.RootPath))
        {
            throw new ArgumentException("RootPath is required.", nameof(criteria));
        }

        var root = PathUtil.NormalizeLongPath(criteria.RootPath);
        if (!Directory.Exists(root))
        {
            throw new DirectoryNotFoundException($"Search root does not exist: {criteria.RootPath}");
        }

        var results = new List<FileMetadata>();
        var dirs = new Stack<string>();
        dirs.Push(root);

        while (dirs.Count > 0 && results.Count < criteria.MaxResults)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var current = dirs.Pop();

            IEnumerable<string> files;
            try
            {
                files = Directory.EnumerateFiles(current);
            }
            catch (Exception ex) when (ex is UnauthorizedAccessException or IOException)
            {
                _logger.LogDebug(ex, "Skipping file enumeration for {Path}", current);
                continue;
            }

            foreach (var file in files)
            {
                cancellationToken.ThrowIfCancellationRequested();

                if (!MatchesCriteria(file, criteria))
                {
                    continue;
                }

                if (criteria.ContentPattern is not null && !await ContentMatchesAsync(file, criteria.ContentPattern, cancellationToken).ConfigureAwait(false))
                {
                    continue;
                }

                FileInfo info;
                try
                {
                    info = new FileInfo(file);
                    if (!criteria.IncludeHiddenFiles && info.Attributes.HasFlag(FileAttributes.Hidden))
                    {
                        continue;
                    }
                }
                catch (Exception ex) when (ex is UnauthorizedAccessException or IOException)
                {
                    _logger.LogDebug(ex, "Skipping metadata read for {Path}", file);
                    continue;
                }

                string? sha = null;
                try
                {
                    sha = await _fileHasher.ComputeSha256Async(file, cancellationToken).ConfigureAwait(false);
                }
                catch (Exception ex) when (ex is IOException or UnauthorizedAccessException)
                {
                    _logger.LogDebug(ex, "Hash unavailable for {Path}", file);
                }

                results.Add(FileMetadata.FromFileInfo(info, sha));
                if (results.Count >= criteria.MaxResults)
                {
                    break;
                }
            }

            if (!criteria.Recursive)
            {
                continue;
            }

            IEnumerable<string> subdirs;
            try
            {
                subdirs = Directory.EnumerateDirectories(current);
            }
            catch (Exception ex) when (ex is UnauthorizedAccessException or IOException)
            {
                _logger.LogDebug(ex, "Skipping directory enumeration for {Path}", current);
                continue;
            }

            foreach (var subdir in subdirs)
            {
                dirs.Push(subdir);
            }

            // Cooperative yielding on large traversals.
            await Task.Yield();
        }

        return results;
    }

    private static bool MatchesCriteria(string path, SearchCriteria criteria)
    {
        var fileName = Path.GetFileName(path);
        if (!WildcardMatch(fileName, criteria.Wildcards))
        {
            return false;
        }

        if (criteria.Extensions.Count > 0)
        {
            var ext = Path.GetExtension(fileName);
            if (!criteria.Extensions.Any(e => string.Equals(NormalizeExt(e), ext, StringComparison.OrdinalIgnoreCase)))
            {
                return false;
            }
        }

        return true;
    }

    private static string NormalizeExt(string ext) => ext.StartsWith('.') ? ext : "." + ext;

    private static bool WildcardMatch(string fileName, IReadOnlyCollection<string> wildcards)
    {
        foreach (var wildcard in wildcards)
        {
            var regex = "^" + Regex.Escape(wildcard).Replace("\\*", ".*").Replace("\\?", ".") + "$";
            if (Regex.IsMatch(fileName, regex, RegexOptions.IgnoreCase | RegexOptions.CultureInvariant))
            {
                return true;
            }
        }

        return false;
    }

    private static async Task<bool> ContentMatchesAsync(string path, Regex pattern, CancellationToken cancellationToken)
    {
        try
        {
            var content = await File.ReadAllTextAsync(path, cancellationToken).ConfigureAwait(false);
            return pattern.IsMatch(content);
        }
        catch (Exception ex) when (ex is UnauthorizedAccessException or IOException)
        {
            return false;
        }
    }
}
