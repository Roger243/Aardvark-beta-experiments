using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using SecurityIdentityModule.Models;

namespace SecurityIdentityModule;

public interface ISecurityManager
{
    Task<bool> AuthorizeAsync(AccessRequest request, CancellationToken cancellationToken = default);
    Task StoreSecretAsync(string key, string value, CancellationToken cancellationToken = default);
    Task<string?> RetrieveSecretAsync(string key, CancellationToken cancellationToken = default);
    Task<bool> ValidateSessionAsync(CancellationToken cancellationToken = default);
    Task<bool> ValidateCommandAsync(string command, CancellationToken cancellationToken = default);
    Task LogSecurityEventAsync(string eventName, string detail, ExecutionLogEntry? sourceEntry = null, CancellationToken cancellationToken = default);
}
