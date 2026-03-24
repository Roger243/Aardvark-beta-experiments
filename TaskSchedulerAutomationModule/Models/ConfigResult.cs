namespace TaskSchedulerAutomationModule.Models;

public sealed record ConfigResult(bool Success, string Message, string? ErrorCode = null)
{
    public static ConfigResult Ok(string message) => new(true, message);
    public static ConfigResult Fail(string message, string? errorCode = null) => new(false, message, errorCode);
}
