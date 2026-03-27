using System.Threading;
using System.Threading.Tasks;
using TaskSchedulerAutomationModule.Models;
using TaskSchedulerAutomationModule.Security;

namespace TaskSchedulerAutomationModule.Tasks;

public interface ITaskProvider
{
    Task<bool> ExistsAsync(string taskName, CancellationToken cancellationToken = default);
    Task<ScheduledTaskInfo?> GetAsync(string taskName, CancellationToken cancellationToken = default);
    Task CreateSimpleTaskAsync(
        string taskName,
        TaskActionConfig action,
        TaskTriggerConfig trigger,
        TaskCredentials credentials,
        string? description = null,
        CancellationToken cancellationToken = default);
    Task DeleteAsync(string taskName, CancellationToken cancellationToken = default);
    Task RunAsync(string taskName, CancellationToken cancellationToken = default);
}
