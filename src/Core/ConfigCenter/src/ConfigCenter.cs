using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ConfigCenter;

public sealed class ConfigCenter : IConfigCenter
{
    public string Name => "ConfigCenter";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
