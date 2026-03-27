using System;
using System.Threading;
using System.Threading.Tasks;
using SelfDiagnosticModule.Models;
using UserUiNotificationModule.Events;
using UserUiNotificationModule.Models;

namespace UserUiNotificationModule;

public interface IUiManager : IAsyncDisposable
{
    event EventHandler<NotificationActionArgs>? NotificationActionInvoked;

    Task NotifyAsync(NotificationContent content, CancellationToken cancellationToken = default);

    Task<bool> PromptUserYesNoAsync(string question, CancellationToken cancellationToken = default);

    Task NotifyHealthIssueAsync(DiagnosticReport report, CancellationToken cancellationToken = default);

    Task SetTrayStatusAsync(TrayStatus status, string? tooltip = null, CancellationToken cancellationToken = default);

    Task PlayCompletionSoundAsync(CancellationToken cancellationToken = default);

    Task PlayCriticalAlertSoundAsync(CancellationToken cancellationToken = default);
}
