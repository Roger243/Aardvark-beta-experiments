using System;
using System.Threading;
using System.Threading.Tasks;
using RegistryConfigModule.Exceptions;
using RegistryConfigModule.Models;
using RegistryConfigModule.Registry;

namespace RegistryConfigModule;

public sealed class RegistryService : IConfigManager
{
    private readonly IRegistryProvider _registryProvider;
    private readonly RegistryBackupManager _backupManager;

    public RegistryService(IRegistryProvider registryProvider, RegistryBackupManager backupManager)
    {
        _registryProvider = registryProvider;
        _backupManager = backupManager;
    }

    public async Task<ConfigChangeResult> SetAsync(string path, string name, object? value, CancellationToken cancellationToken = default)
    {
        try
        {
            var backupPath = await TrySnapshotIfExistsAsync(path, cancellationToken).ConfigureAwait(false);
            var result = await _registryProvider.WriteValueAsync(path, name, value, cancellationToken).ConfigureAwait(false);
            return result with { BackupPath = backupPath ?? result.BackupPath };
        }
        catch (RegistryModuleException ex)
        {
            return ConfigChangeResult.Fail(ex.Message, errorCode: "REGISTRY_ERROR");
        }
        catch (UnauthorizedAccessException ex)
        {
            return ConfigChangeResult.Fail(ex.Message, errorCode: "ACCESS_DENIED");
        }
    }

    public async Task<object?> GetAsync(string path, string name, object? defaultValue = null, CancellationToken cancellationToken = default)
    {
        try
        {
            return await _registryProvider.ReadValueAsync(path, name, defaultValue, cancellationToken).ConfigureAwait(false);
        }
        catch (RegistryModuleException)
        {
            return defaultValue;
        }
    }

    public async Task<ConfigChangeResult> DeleteValueAsync(string path, string name, CancellationToken cancellationToken = default)
    {
        try
        {
            var backupPath = await TrySnapshotIfExistsAsync(path, cancellationToken).ConfigureAwait(false);
            var result = await _registryProvider.DeleteValueAsync(path, name, cancellationToken).ConfigureAwait(false);
            return result with { BackupPath = backupPath ?? result.BackupPath };
        }
        catch (RegistryModuleException ex)
        {
            return ConfigChangeResult.Fail(ex.Message, errorCode: "REGISTRY_ERROR");
        }
    }

    public async Task<ConfigChangeResult> DeleteKeyAsync(string path, bool recursive = true, CancellationToken cancellationToken = default)
    {
        try
        {
            var backupPath = await TrySnapshotIfExistsAsync(path, cancellationToken).ConfigureAwait(false);
            var result = await _registryProvider.DeleteKeyAsync(path, recursive, cancellationToken).ConfigureAwait(false);
            return result with { BackupPath = backupPath ?? result.BackupPath };
        }
        catch (RegistryModuleException ex)
        {
            return ConfigChangeResult.Fail(ex.Message, errorCode: "REGISTRY_ERROR");
        }
    }

    private async Task<string?> TrySnapshotIfExistsAsync(string path, CancellationToken cancellationToken)
    {
        if (!await _registryProvider.KeyExistsAsync(path, cancellationToken).ConfigureAwait(false))
        {
            return null;
        }

        return await _backupManager.SnapshotKeyAsync(path, cancellationToken).ConfigureAwait(false);
    }
}
