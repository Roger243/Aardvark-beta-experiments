using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ClusterSync;

public sealed class ModuleService : IModule
{
    public string Name => "ClusterSync";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
