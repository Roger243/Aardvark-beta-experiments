using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.DiagnosticsBridge;

public sealed class DiagnosticsBridge : IDiagnosticsBridge
{
    public string Name => "DiagnosticsBridge";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
