using System;
using System.Collections.Concurrent;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using SecurityIdentityModule.Exceptions;

namespace SecurityIdentityModule.Vault;

public sealed class DpapiVaultProvider : ISecretVault
{
    private readonly DataProtectionScope _scope;
    private readonly byte[] _entropy;
    private readonly ConcurrentDictionary<string, byte[]> _vault = new(StringComparer.OrdinalIgnoreCase);

    public DpapiVaultProvider(DataProtectionScope scope = DataProtectionScope.CurrentUser, byte[]? entropy = null)
    {
        _scope = scope;
        _entropy = entropy ?? Encoding.UTF8.GetBytes("WinAgent.SecurityIdentityModule.v1");
    }

    public Task ProtectAsync(string key, string plaintext, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (string.IsNullOrWhiteSpace(key))
        {
            throw new SecurityModuleException("Secret key cannot be empty.");
        }

        try
        {
            var clearBytes = Encoding.UTF8.GetBytes(plaintext ?? string.Empty);
            var cipherBytes = ProtectedData.Protect(clearBytes, _entropy, _scope);
            _vault[key] = cipherBytes;
            CryptographicOperations.ZeroMemory(clearBytes);

            return Task.CompletedTask;
        }
        catch (CryptographicException ex)
        {
            throw new SecurityModuleException("Failed to protect secret with DPAPI.", ex);
        }
    }

    public Task<string?> UnprotectAsync(string key, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (!_vault.TryGetValue(key, out var cipherBytes))
        {
            return Task.FromResult<string?>(null);
        }

        try
        {
            var clearBytes = ProtectedData.Unprotect(cipherBytes, _entropy, _scope);
            var plaintext = Encoding.UTF8.GetString(clearBytes);
            CryptographicOperations.ZeroMemory(clearBytes);
            return Task.FromResult<string?>(plaintext);
        }
        catch (CryptographicException ex)
        {
            throw new SecurityModuleException("Failed to unprotect secret. Vault data may be corrupted.", ex);
        }
    }
}
