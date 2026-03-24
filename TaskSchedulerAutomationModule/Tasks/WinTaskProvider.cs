using System;
using System.Security;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32.TaskScheduler;
using TaskSchedulerAutomationModule.Exceptions;
using TaskSchedulerAutomationModule.Models;
using TaskSchedulerAutomationModule.Security;
using TaskSchedulerAutomationModule.Triggers;

namespace TaskSchedulerAutomationModule.Tasks;

public sealed class WinTaskProvider : ITaskProvider
{
    public Task<bool> ExistsAsync(string taskName, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var taskService = new TaskService();
        var task = taskService.GetTask(taskName);
        return Task.FromResult(task is not null);
    }

    public Task<ScheduledTaskInfo?> GetAsync(string taskName, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var taskService = new TaskService();
        var task = taskService.GetTask(taskName);
        if (task is null)
        {
            return Task.FromResult<ScheduledTaskInfo?>(null);
        }

        return Task.FromResult<ScheduledTaskInfo?>(new ScheduledTaskInfo(
            Name: task.Name,
            Path: task.Path,
            Author: task.Definition.RegistrationInfo.Author,
            State: task.State.ToString(),
            LastRunTime: task.LastRunTime == DateTime.MinValue ? null : task.LastRunTime,
            NextRunTime: task.NextRunTime == DateTime.MinValue ? null : task.NextRunTime,
            LastTaskResult: task.LastTaskResult));
    }

    public Task CreateSimpleTaskAsync(
        string taskName,
        TaskActionConfig action,
        TaskTriggerConfig trigger,
        TaskCredentials credentials,
        string? description = null,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            using var taskService = new TaskService();
            var definition = taskService.NewTask();

            definition.RegistrationInfo.Description = description ?? $"Automated task created by WinAgent: {taskName}";
            definition.RegistrationInfo.Author = "WinAgent";
            definition.Settings.StartWhenAvailable = true;
            definition.Settings.AllowHardTerminate = true;
            definition.Settings.DisallowStartIfOnBatteries = false;
            definition.Settings.StopIfGoingOnBatteries = false;
            definition.Settings.MultipleInstances = TaskInstancesPolicy.IgnoreNew;

            definition.Triggers.Add(TriggerFactory.Create(trigger));
            definition.Actions.Add(new ExecAction(action.ExecutablePath, action.Arguments, action.WorkingDirectory));

            var (userId, logonType, runLevel) = ResolvePrincipal(credentials);
            definition.Principal.UserId = userId;
            definition.Principal.LogonType = logonType;
            definition.Principal.RunLevel = runLevel;

            taskService.RootFolder.RegisterTaskDefinition(
                taskName,
                definition,
                TaskCreation.CreateOrUpdate,
                userId,
                credentials.Password,
                logonType,
                null);

            return Task.CompletedTask;
        }
        catch (UnauthorizedAccessException ex)
        {
            throw new SchedulerModuleException($"Access denied while creating task '{taskName}'.", ex);
        }
        catch (SecurityException ex)
        {
            throw new SchedulerModuleException($"Security policy blocked task creation for '{taskName}'.", ex);
        }
        catch (Exception ex)
        {
            throw new SchedulerModuleException($"Failed to create scheduled task '{taskName}'.", ex);
        }
    }

    public Task DeleteAsync(string taskName, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            using var taskService = new TaskService();
            taskService.RootFolder.DeleteTask(taskName, exceptionOnNotExists: false);
            return Task.CompletedTask;
        }
        catch (UnauthorizedAccessException ex)
        {
            throw new SchedulerModuleException($"Access denied deleting task '{taskName}'.", ex);
        }
    }

    public Task RunAsync(string taskName, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        using var taskService = new TaskService();
        var task = taskService.GetTask(taskName)
            ?? throw new SchedulerModuleException($"Task not found: {taskName}");

        task.Run();
        return Task.CompletedTask;
    }

    private static (string? UserId, TaskLogonType LogonType, TaskRunLevel RunLevel) ResolvePrincipal(TaskCredentials credentials)
        => credentials.RunContext switch
        {
            TaskRunContext.System => ("SYSTEM", TaskLogonType.ServiceAccount, TaskRunLevel.Highest),
            TaskRunContext.LoggedOnUser => (
                credentials.UserId,
                TaskLogonType.InteractiveToken,
                TaskRunLevel.LUA),
            _ => throw new ArgumentOutOfRangeException(nameof(credentials.RunContext), "Unsupported task run context.")
        };
}
