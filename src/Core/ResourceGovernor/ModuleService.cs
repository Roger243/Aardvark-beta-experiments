using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ResourceGovernor;

public sealed class ModuleService : IModule
{
    public string Name => "ResourceGovernor";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
