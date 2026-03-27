using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.TelemetryHub;

public sealed class TelemetryHub : ITelemetryHub
{
    public string Name => "TelemetryHub";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
