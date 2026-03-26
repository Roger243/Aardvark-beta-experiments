using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.DynamicLoader;

public sealed class ModuleService : IModule
{
    public string Name => "DynamicLoader";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
