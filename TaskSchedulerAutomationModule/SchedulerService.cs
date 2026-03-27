using System;
using System.Threading;
using System.Threading.Tasks;
using TaskSchedulerAutomationModule.Exceptions;
using TaskSchedulerAutomationModule.Models;
using TaskSchedulerAutomationModule.Security;
using TaskSchedulerAutomationModule.Tasks;

namespace TaskSchedulerAutomationModule;

public sealed class SchedulerService : ISchedulerManager
{
    private readonly ITaskProvider _taskProvider;

    public SchedulerService(ITaskProvider taskProvider)
    {
        _taskProvider = taskProvider;
    }

    public Task<bool> ExistsAsync(string taskName, CancellationToken cancellationToken = default)
        => _taskProvider.ExistsAsync(taskName, cancellationToken);

    public Task<ScheduledTaskInfo?> GetAsync(string taskName, CancellationToken cancellationToken = default)
        => _taskProvider.GetAsync(taskName, cancellationToken);

    public async Task<ConfigResult> CreateOrUpdateAsync(
        string taskName,
        TaskActionConfig action,
        TaskTriggerConfig trigger,
        TaskCredentials credentials,
        string? description = null,
        CancellationToken cancellationToken = default)
    {
        try
        {
            await _taskProvider
                .CreateSimpleTaskAsync(taskName, action, trigger, credentials, description, cancellationToken)
                .ConfigureAwait(false);

            return ConfigResult.Ok($"Scheduled task created/updated: {taskName}");
        }
        catch (SchedulerModuleException ex)
        {
            return ConfigResult.Fail(ex.Message, "SCHEDULER_ERROR");
        }
        catch (UnauthorizedAccessException ex)
        {
            return ConfigResult.Fail(ex.Message, "ACCESS_DENIED");
        }
    }

    public async Task<ConfigResult> DeleteAsync(string taskName, CancellationToken cancellationToken = default)
    {
        try
        {
            await _taskProvider.DeleteAsync(taskName, cancellationToken).ConfigureAwait(false);
            return ConfigResult.Ok($"Scheduled task deleted: {taskName}");
        }
        catch (SchedulerModuleException ex)
        {
            return ConfigResult.Fail(ex.Message, "SCHEDULER_ERROR");
        }
        catch (UnauthorizedAccessException ex)
        {
            return ConfigResult.Fail(ex.Message, "ACCESS_DENIED");
        }
    }

    public async Task<ConfigResult> RunNowAsync(string taskName, CancellationToken cancellationToken = default)
    {
        try
        {
            await _taskProvider.RunAsync(taskName, cancellationToken).ConfigureAwait(false);
            return ConfigResult.Ok($"Scheduled task started: {taskName}");
        }
        catch (SchedulerModuleException ex)
        {
            return ConfigResult.Fail(ex.Message, "TASK_NOT_FOUND");
        }
    }
}
