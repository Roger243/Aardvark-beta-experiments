using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.HealthCoordinator;

public sealed class HealthCoordinator : IHealthCoordinator
{
    public string Name => "HealthCoordinator";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
