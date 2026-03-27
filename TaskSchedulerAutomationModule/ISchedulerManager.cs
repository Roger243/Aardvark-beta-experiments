using System.Threading;
using System.Threading.Tasks;
using TaskSchedulerAutomationModule.Models;
using TaskSchedulerAutomationModule.Security;

namespace TaskSchedulerAutomationModule;

public interface ISchedulerManager
{
    Task<bool> ExistsAsync(string taskName, CancellationToken cancellationToken = default);
    Task<ScheduledTaskInfo?> GetAsync(string taskName, CancellationToken cancellationToken = default);
    Task<ConfigResult> CreateOrUpdateAsync(
        string taskName,
        TaskActionConfig action,
        TaskTriggerConfig trigger,
        TaskCredentials credentials,
        string? description = null,
        CancellationToken cancellationToken = default);
    Task<ConfigResult> DeleteAsync(string taskName, CancellationToken cancellationToken = default);
    Task<ConfigResult> RunNowAsync(string taskName, CancellationToken cancellationToken = default);
}
