using System;

namespace SystemMonitorModule.Exceptions;

public sealed class MonitorException : Exception
{
    public MonitorException(string message, Exception? inner = null) : base(message, inner) { }
}
