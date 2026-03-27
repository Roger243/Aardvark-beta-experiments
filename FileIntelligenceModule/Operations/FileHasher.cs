using System;
using System.IO;
using System.Security.Cryptography;
using System.Threading;
using System.Threading.Tasks;

namespace FileIntelligenceModule.Operations;

public sealed class FileHasher
{
    public async Task<string> ComputeSha256Async(string path, CancellationToken cancellationToken = default)
    {
        var normalized = PathUtil.NormalizeLongPath(path);
        await using var stream = new FileStream(normalized, FileMode.Open, FileAccess.Read, FileShare.ReadWrite | FileShare.Delete);
        using var sha = SHA256.Create();
        var hash = await sha.ComputeHashAsync(stream, cancellationToken).ConfigureAwait(false);
        return Convert.ToHexString(hash);
    }
}
