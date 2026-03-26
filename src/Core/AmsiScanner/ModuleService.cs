using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.AmsiScanner;

public sealed class ModuleService : IModule
{
    public string Name => "AmsiScanner";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
