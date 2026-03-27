using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.VirtualDisk;

public sealed class ModuleService : IModule
{
    public string Name => "VirtualDisk";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
