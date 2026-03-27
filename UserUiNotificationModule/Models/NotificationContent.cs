using System;

namespace UserUiNotificationModule.Models;

public sealed record NotificationContent(
    string Title,
    string Message,
    DateTimeOffset? ExpirationUtc,
    NotificationType Type);

public enum NotificationType
{
    Info,
    Success,
    Warning,
    Critical
}
