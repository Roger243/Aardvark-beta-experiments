using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ModuleRegistry;

public sealed class ModuleRegistry : IModuleRegistry
{
    public string Name => "ModuleRegistry";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
