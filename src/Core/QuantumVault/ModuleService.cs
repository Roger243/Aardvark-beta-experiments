using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.QuantumVault;

public sealed class ModuleService : IModule
{
    public string Name => "QuantumVault";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
