using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.TaskRouter;

public sealed class TaskRouter : ITaskRouter
{
    public string Name => "TaskRouter";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
