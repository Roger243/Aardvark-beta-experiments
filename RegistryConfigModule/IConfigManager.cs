using System.Threading;
using System.Threading.Tasks;
using RegistryConfigModule.Models;

namespace RegistryConfigModule;

public interface IConfigManager
{
    Task<ConfigChangeResult> SetAsync(string path, string name, object? value, CancellationToken cancellationToken = default);
    Task<object?> GetAsync(string path, string name, object? defaultValue = null, CancellationToken cancellationToken = default);
    Task<ConfigChangeResult> DeleteValueAsync(string path, string name, CancellationToken cancellationToken = default);
    Task<ConfigChangeResult> DeleteKeyAsync(string path, bool recursive = true, CancellationToken cancellationToken = default);
}
