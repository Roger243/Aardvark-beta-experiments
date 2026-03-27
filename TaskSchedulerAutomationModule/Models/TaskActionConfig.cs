namespace TaskSchedulerAutomationModule.Models;

public sealed record TaskActionConfig(
    string ExecutablePath,
    string? Arguments = null,
    string? WorkingDirectory = null);
