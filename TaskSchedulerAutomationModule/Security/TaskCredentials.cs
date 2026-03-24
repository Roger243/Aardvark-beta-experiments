namespace TaskSchedulerAutomationModule.Security;

public enum TaskRunContext
{
    LoggedOnUser,
    System
}

public sealed record TaskCredentials(TaskRunContext RunContext, string? UserId = null, string? Password = null);
