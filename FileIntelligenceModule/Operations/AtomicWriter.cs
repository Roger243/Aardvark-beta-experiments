using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using FileIntelligenceModule.Exceptions;

namespace FileIntelligenceModule.Operations;

public sealed class AtomicWriter
{
    public async Task WriteAllTextAsync(string path, string content, CancellationToken cancellationToken = default)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);

        var normalized = PathUtil.NormalizeLongPath(path);
        var directory = Path.GetDirectoryName(normalized);
        if (string.IsNullOrWhiteSpace(directory))
        {
            throw new FileIntelligenceException($"Invalid target directory for path: {path}");
        }

        Directory.CreateDirectory(directory);

        var fileName = Path.GetFileName(normalized);
        var tempPath = Path.Combine(directory, $".{fileName}.{Guid.NewGuid():N}.tmp");
        var backupPath = Path.Combine(directory, $".{fileName}.{DateTime.UtcNow:yyyyMMddHHmmss}.bak");

        try
        {
            if (File.Exists(normalized))
            {
                File.Copy(normalized, backupPath, overwrite: true);
            }

            await File.WriteAllTextAsync(tempPath, content, cancellationToken).ConfigureAwait(false);

            if (File.Exists(normalized))
            {
                // Replace keeps original ACL/metadata better than move-overwrite.
                File.Replace(tempPath, normalized, backupPath, ignoreMetadataErrors: true);
            }
            else
            {
                File.Move(tempPath, normalized);
            }
        }
        catch (UnauthorizedAccessException ex)
        {
            throw new FileIntelligenceException($"Access denied while writing '{path}'.", ex);
        }
        catch (IOException ex)
        {
            throw new FileIntelligenceException($"I/O failure (possibly locked/in-use file) for '{path}'.", ex);
        }
        finally
        {
            try
            {
                if (File.Exists(tempPath))
                {
                    File.Delete(tempPath);
                }
            }
            catch
            {
                // Best-effort cleanup.
            }
        }
    }
}
