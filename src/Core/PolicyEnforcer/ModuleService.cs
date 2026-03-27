using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.PolicyEnforcer;

public sealed class ModuleService : IModule
{
    public string Name => "PolicyEnforcer";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
