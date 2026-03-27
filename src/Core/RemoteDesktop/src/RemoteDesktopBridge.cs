using System.Threading;
using System.Threading.Tasks;
using WinAgent.Core;

namespace WinAgent.Core.RemoteDesktop;

public sealed class RemoteDesktopBridge : IModule
{
    public string Name => "RemoteDesktopBridge";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
