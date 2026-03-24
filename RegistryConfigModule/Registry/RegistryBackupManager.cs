using System;
using System.Collections.Generic;
using System.IO;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32;
using RegistryConfigModule.Exceptions;
using RegistryConfigModule.Models;
using RegistryConfigModule.Utils;

namespace RegistryConfigModule.Registry;

public sealed class RegistryBackupManager
{
    private readonly string _backupRoot;

    public RegistryBackupManager(string? backupRoot = null)
    {
        _backupRoot = backupRoot ?? Path.Combine(AppContext.BaseDirectory, "registry-backups");
    }

    public async Task<string> SnapshotKeyAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var snapshot = CaptureSnapshot(path);
        Directory.CreateDirectory(_backupRoot);

        var fileName = $"{Sanitize(path)}_{DateTime.UtcNow:yyyyMMdd_HHmmssfff}.json";
        var outputPath = Path.Combine(_backupRoot, fileName);

        await using var stream = File.Create(outputPath);
        await JsonSerializer.SerializeAsync(stream, snapshot, cancellationToken: cancellationToken).ConfigureAwait(false);

        return outputPath;
    }

    private static RegistryKeyInfo CaptureSnapshot(string path)
    {
        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            using var root = hive.OpenSubKey(subKeyPath, writable: false)
                ?? throw new RegistryModuleException($"Registry key not found for backup: {path}");

            return CaptureRecursive(path, root);
        }
        catch (UnauthorizedAccessException ex)
        {
            throw new RegistryModuleException($"Access denied while creating backup for '{path}'.", ex);
        }
    }

    private static RegistryKeyInfo CaptureRecursive(string fullPath, RegistryKey key)
    {
        var values = new Dictionary<string, object?>(StringComparer.OrdinalIgnoreCase);
        foreach (var valueName in key.GetValueNames())
        {
            values[valueName] = key.GetValue(valueName);
        }

        var children = new List<RegistryKeyInfo>();
        foreach (var childName in key.GetSubKeyNames())
        {
            using var child = key.OpenSubKey(childName, writable: false);
            if (child is null)
            {
                continue;
            }

            children.Add(CaptureRecursive($"{fullPath}\\{childName}", child));
        }

        return new RegistryKeyInfo(fullPath, values, children);
    }

    private static string Sanitize(string path)
    {
        var invalid = Path.GetInvalidFileNameChars();
        var sanitized = path;
        foreach (var ch in invalid)
        {
            sanitized = sanitized.Replace(ch, '_');
        }

        return sanitized.Replace('\\', '_').Replace('/', '_').Replace(':', '_');
    }
}
