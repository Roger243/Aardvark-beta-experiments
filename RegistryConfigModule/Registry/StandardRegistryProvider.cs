using System;
using System.Security;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32;
using RegistryConfigModule.Exceptions;
using RegistryConfigModule.Models;
using RegistryConfigModule.Utils;

namespace RegistryConfigModule.Registry;

public sealed class StandardRegistryProvider : IRegistryProvider
{
    public Task<object?> ReadValueAsync(string path, string valueName, object? defaultValue = null, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            using var key = hive.OpenSubKey(subKeyPath, writable: false);
            return Task.FromResult(key?.GetValue(valueName, defaultValue));
        }
        catch (UnauthorizedAccessException ex)
        {
            throw new RegistryModuleException($"Access denied reading registry value '{path}\\{valueName}'.", ex);
        }
        catch (SecurityException ex)
        {
            throw new RegistryModuleException($"Security policy blocked read for '{path}\\{valueName}'.", ex);
        }
    }

    public Task<ConfigChangeResult> WriteValueAsync(string path, string valueName, object? value, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            using var key = hive.CreateSubKey(subKeyPath, writable: true)
                ?? throw new RegistryModuleException($"Unable to create/open key '{path}'.");

            var valueKind = ResolveValueKind(value);
            key.SetValue(valueName, value ?? string.Empty, valueKind);

            return Task.FromResult(ConfigChangeResult.Ok($"Registry value written: {path}\\{valueName}"));
        }
        catch (UnauthorizedAccessException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Access denied writing '{path}\\{valueName}'.",
                errorCode: "ACCESS_DENIED"));
        }
        catch (SecurityException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Security policy blocked write for '{path}\\{valueName}'.",
                errorCode: "SECURITY_BLOCK"));
        }
    }

    public Task<ConfigChangeResult> DeleteValueAsync(string path, string valueName, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            using var key = hive.OpenSubKey(subKeyPath, writable: true);
            if (key is null)
            {
                return Task.FromResult(ConfigChangeResult.Fail($"Registry key not found: {path}", "KEY_NOT_FOUND"));
            }

            key.DeleteValue(valueName, throwOnMissingValue: false);
            return Task.FromResult(ConfigChangeResult.Ok($"Registry value deleted: {path}\\{valueName}"));
        }
        catch (UnauthorizedAccessException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Access denied deleting value '{path}\\{valueName}'.",
                errorCode: "ACCESS_DENIED"));
        }
        catch (SecurityException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Security policy blocked delete for '{path}\\{valueName}'.",
                errorCode: "SECURITY_BLOCK"));
        }
    }

    public Task<ConfigChangeResult> DeleteKeyAsync(string path, bool recursive = true, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            if (recursive)
            {
                hive.DeleteSubKeyTree(subKeyPath, throwOnMissingSubKey: false);
            }
            else
            {
                hive.DeleteSubKey(subKeyPath, throwOnMissingSubKey: false);
            }

            return Task.FromResult(ConfigChangeResult.Ok($"Registry key deleted: {path}"));
        }
        catch (UnauthorizedAccessException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Access denied deleting key '{path}'.",
                errorCode: "ACCESS_DENIED"));
        }
        catch (SecurityException)
        {
            return Task.FromResult(ConfigChangeResult.Fail(
                $"Security policy blocked delete for '{path}'.",
                errorCode: "SECURITY_BLOCK"));
        }
    }

    public Task<bool> KeyExistsAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var (hive, subKeyPath) = RegistryPathParser.Parse(path);
            using var key = hive.OpenSubKey(subKeyPath, writable: false);
            return Task.FromResult(key is not null);
        }
        catch (UnauthorizedAccessException)
        {
            return Task.FromResult(false);
        }
        catch (SecurityException)
        {
            return Task.FromResult(false);
        }
    }

    private static RegistryValueKind ResolveValueKind(object? value)
        => value switch
        {
            null => RegistryValueKind.String,
            int => RegistryValueKind.DWord,
            long => RegistryValueKind.QWord,
            byte[] => RegistryValueKind.Binary,
            string[] => RegistryValueKind.MultiString,
            string => RegistryValueKind.String,
            _ => RegistryValueKind.String
        };
}
