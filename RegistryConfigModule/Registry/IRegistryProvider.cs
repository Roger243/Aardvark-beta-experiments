using System.Threading;
using System.Threading.Tasks;
using RegistryConfigModule.Models;

namespace RegistryConfigModule.Registry;

public interface IRegistryProvider
{
    Task<object?> ReadValueAsync(string path, string valueName, object? defaultValue = null, CancellationToken cancellationToken = default);
    Task<ConfigChangeResult> WriteValueAsync(string path, string valueName, object? value, CancellationToken cancellationToken = default);
    Task<ConfigChangeResult> DeleteValueAsync(string path, string valueName, CancellationToken cancellationToken = default);
    Task<ConfigChangeResult> DeleteKeyAsync(string path, bool recursive = true, CancellationToken cancellationToken = default);
    Task<bool> KeyExistsAsync(string path, CancellationToken cancellationToken = default);
}
