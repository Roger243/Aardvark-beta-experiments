using System;

namespace UserUiNotificationModule.Events;

public sealed class NotificationActionArgs : EventArgs
{
    public static NotificationActionArgs None { get; } = new("", "");

    public NotificationActionArgs(string actionId, string? userInput)
    {
        ActionId = actionId;
        UserInput = userInput;
    }

    public string ActionId { get; }

    public string? UserInput { get; }
}
