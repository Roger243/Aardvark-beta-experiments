using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ZstdCompression;

public sealed class ModuleService : IModule
{
    public string Name => "ZstdCompression";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
