using System;
using Microsoft.Win32;

namespace RegistryConfigModule.Utils;

public static class RegistryPathParser
{
    public static (RegistryKey Hive, string SubKeyPath) Parse(string path)
    {
        if (string.IsNullOrWhiteSpace(path))
        {
            throw new ArgumentException("Registry path cannot be empty.", nameof(path));
        }

        var normalized = path.Replace('/', '\\').Trim();
        var parts = normalized.Split('\\', 2, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length == 0)
        {
            throw new ArgumentException("Invalid registry path.", nameof(path));
        }

        var hive = parts[0].ToUpperInvariant() switch
        {
            "HKLM" or "HKEY_LOCAL_MACHINE" => Registry.LocalMachine,
            "HKCU" or "HKEY_CURRENT_USER" => Registry.CurrentUser,
            "HKCR" or "HKEY_CLASSES_ROOT" => Registry.ClassesRoot,
            "HKU" or "HKEY_USERS" => Registry.Users,
            "HKCC" or "HKEY_CURRENT_CONFIG" => Registry.CurrentConfig,
            _ => throw new ArgumentException($"Unknown registry hive: {parts[0]}", nameof(path))
        };

        var subKeyPath = parts.Length == 2 ? parts[1] : string.Empty;
        return (hive, subKeyPath);
    }
}
