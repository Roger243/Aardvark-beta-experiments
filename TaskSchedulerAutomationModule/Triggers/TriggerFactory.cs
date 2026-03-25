using System;
using System.Linq;
using Microsoft.Win32.TaskScheduler;
using TaskSchedulerAutomationModule.Models;

namespace TaskSchedulerAutomationModule.Triggers;

internal static class TriggerFactory
{
    public static Trigger Create(TaskTriggerConfig config)
        => config.Type switch
        {
            TaskTriggerType.Daily => new DailyTrigger { StartBoundary = config.StartBoundary, DaysInterval = config.DaysInterval },
            TaskTriggerType.Weekly => new WeeklyTrigger
            {
                StartBoundary = config.StartBoundary,
                WeeksInterval = config.WeeksInterval,
                DaysOfWeek = ResolveDays(config.DaysOfWeek)
            },
            TaskTriggerType.Boot => new BootTrigger { Delay = TimeSpan.FromMinutes(1) },
            TaskTriggerType.Event => new EventTrigger(BuildSubscription(config)),
            _ => throw new ArgumentOutOfRangeException(nameof(config), "Unsupported trigger type.")
        };

    private static DaysOfTheWeek ResolveDays(string[]? days)
    {
        if (days is null || days.Length == 0)
        {
            return DaysOfTheWeek.AllDays;
        }

        var resolved = DaysOfTheWeek.Monday;
        resolved = 0;

        foreach (var day in days.Select(d => d.Trim().ToLowerInvariant()))
        {
            resolved |= day switch
            {
                "monday" => DaysOfTheWeek.Monday,
                "tuesday" => DaysOfTheWeek.Tuesday,
                "wednesday" => DaysOfTheWeek.Wednesday,
                "thursday" => DaysOfTheWeek.Thursday,
                "friday" => DaysOfTheWeek.Friday,
                "saturday" => DaysOfTheWeek.Saturday,
                "sunday" => DaysOfTheWeek.Sunday,
                _ => 0
            };
        }

        return resolved == 0 ? DaysOfTheWeek.AllDays : resolved;
    }

    private static string BuildSubscription(TaskTriggerConfig config)
    {
        var logName = string.IsNullOrWhiteSpace(config.EventLogName) ? "Application" : config.EventLogName;
        var eventId = config.EventId ?? 1000;

        return $"<QueryList><Query Id='0' Path='{logName}'><Select Path='{logName}'>*[System[(EventID={eventId})]]</Select></Query></QueryList>";
    }
}
