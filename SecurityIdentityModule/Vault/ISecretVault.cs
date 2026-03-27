using System.Threading;
using System.Threading.Tasks;

namespace SecurityIdentityModule.Vault;

public interface ISecretVault
{
    Task ProtectAsync(string key, string plaintext, CancellationToken cancellationToken = default);
    Task<string?> UnprotectAsync(string key, CancellationToken cancellationToken = default);
}
