using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.IdentityGuard;

public sealed class IdentityGuard : IIdentityGuard
{
    public string Name => "IdentityGuard";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
