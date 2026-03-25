using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.PolicyEngine;

public sealed class PolicyEngine : IPolicyEngine
{
    public string Name => "PolicyEngine";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
