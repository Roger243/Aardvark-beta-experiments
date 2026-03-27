using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.NotificationCenter;

public sealed class NotificationCenter : INotificationCenter
{
    public string Name => "NotificationCenter";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
