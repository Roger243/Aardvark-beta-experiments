using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.AgentHost;

public sealed class AgentHost : IAgentHost
{
    public string Name => "AgentHost";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
