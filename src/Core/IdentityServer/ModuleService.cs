using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.IdentityServer;

public sealed class ModuleService : IModule
{
    public string Name => "IdentityServer";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
