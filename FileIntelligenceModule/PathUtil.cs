using System;
using System.IO;

namespace FileIntelligenceModule;

internal static class PathUtil
{
    public static string NormalizeLongPath(string path)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(path);

        var full = Path.GetFullPath(path);

        if (!OperatingSystem.IsWindows())
        {
            return full;
        }

        if (full.StartsWith("\\\\?\\", StringComparison.Ordinal))
        {
            return full;
        }

        if (full.StartsWith("\\\\", StringComparison.Ordinal))
        {
            // UNC path.
            return "\\\\?\\UNC\\" + full.TrimStart('\\');
        }

        return "\\\\?\\" + full;
    }
}
