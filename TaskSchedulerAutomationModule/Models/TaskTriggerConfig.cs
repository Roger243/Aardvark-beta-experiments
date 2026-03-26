using System;

namespace TaskSchedulerAutomationModule.Models;

public enum TaskTriggerType
{
    Daily,
    Weekly,
    Boot,
    Event
}

public sealed record TaskTriggerConfig(
    TaskTriggerType Type,
    DateTime StartBoundary,
    short DaysInterval = 1,
    short WeeksInterval = 1,
    string[]? DaysOfWeek = null,
    string? EventLogName = null,
    int? EventId = null);
