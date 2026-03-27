using System.Threading;
using System.Threading.Tasks;
using WinAgent.Core;

namespace WinAgent.Core.TerminalEngine;

public sealed class TerminalEngineBridge : IModule
{
    public string Name => "TerminalEngineBridge";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
