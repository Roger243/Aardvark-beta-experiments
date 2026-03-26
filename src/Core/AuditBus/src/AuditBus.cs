using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.AuditBus;

public sealed class AuditBus : IAuditBus
{
    public string Name => "AuditBus";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
