using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.SqlEngine;

public sealed class ModuleService : IModule
{
    public string Name => "SqlEngine";

    public Task InitializeAsync(CancellationToken cancellationToken = default)
        => Task.CompletedTask;
}
