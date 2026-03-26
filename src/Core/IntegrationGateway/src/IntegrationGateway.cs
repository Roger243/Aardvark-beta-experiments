using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.IntegrationGateway;

public sealed class IntegrationGateway : IIntegrationGateway
{
    public string Name => "IntegrationGateway";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
