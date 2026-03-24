using System;

namespace TaskSchedulerAutomationModule.Exceptions;

public sealed class SchedulerModuleException : Exception
{
    public SchedulerModuleException(string message) : base(message) { }
    public SchedulerModuleException(string message, Exception innerException) : base(message, innerException) { }
}
