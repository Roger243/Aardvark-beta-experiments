using System;

namespace TaskSchedulerAutomationModule.Models;

public sealed record ScheduledTaskInfo(
    string Name,
    string Path,
    string Author,
    string State,
    DateTime? LastRunTime,
    DateTime? NextRunTime,
    int LastTaskResult);
