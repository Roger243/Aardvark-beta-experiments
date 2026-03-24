using System;
using System.Collections.Generic;
using System.IO;

namespace SelfDiagnosticModule.Storage;

public sealed class IntegrityManifest
{
    private readonly Dictionary<string, string> _expectedHashes;

    public IntegrityManifest(Dictionary<string, string> expectedHashes)
    {
        _expectedHashes = expectedHashes;
    }

    public bool TryGetExpectedHash(string absolutePath, out string? expectedHash)
    {
        var normalized = Path.GetFullPath(absolutePath);
        return _expectedHashes.TryGetValue(normalized, out expectedHash);
    }

    public static IntegrityManifest CreateDefault(string appBasePath, string? executableHash = null)
    {
        var map = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        if (!string.IsNullOrWhiteSpace(executableHash))
        {
            map[Path.Combine(appBasePath, "WinAgent.exe")] = executableHash;
        }

        return new IntegrityManifest(map);
    }
}
