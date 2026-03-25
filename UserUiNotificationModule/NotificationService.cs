using System;
using System.Linq;
using System.Media;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SelfDiagnosticModule.Models;
using UserUiNotificationModule.Dialogs;
using UserUiNotificationModule.Events;
using UserUiNotificationModule.Exceptions;
using UserUiNotificationModule.Models;
using UserUiNotificationModule.Toasts;
using UserUiNotificationModule.Tray;

namespace UserUiNotificationModule;

public sealed class NotificationService : IUiManager
{
    private readonly IToastProvider _toastProvider;
    private readonly IDialogProvider _dialogProvider;
    private readonly TrayIconManager _trayIconManager;
    private readonly ILogger<NotificationService> _logger;

    public NotificationService(
        IToastProvider toastProvider,
        IDialogProvider dialogProvider,
        TrayIconManager trayIconManager,
        ILogger<NotificationService> logger)
    {
        _toastProvider = toastProvider;
        _dialogProvider = dialogProvider;
        _trayIconManager = trayIconManager;
        _logger = logger;
    }

    public event EventHandler<NotificationActionArgs>? NotificationActionInvoked;

    public async Task NotifyAsync(NotificationContent content, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var title = string.IsNullOrWhiteSpace(content.Title) ? "WinAgent" : content.Title;
            var message = content.Message ?? string.Empty;

            if (content.Type == NotificationType.Critical)
            {
                await PlayCriticalAlertSoundAsync(cancellationToken).ConfigureAwait(false);
            }
            else if (content.Type == NotificationType.Success)
            {
                await PlayCompletionSoundAsync(cancellationToken).ConfigureAwait(false);
            }

            await _toastProvider.ShowToastAsync(title, message, cancellationToken).ConfigureAwait(false);
            NotificationActionInvoked?.Invoke(this, NotificationActionArgs.None);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to display notification.");
            throw new UiModuleException("Failed to display notification.", ex);
        }
    }

    public Task<bool> PromptUserYesNoAsync(string question, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        return _dialogProvider.PromptUserYesNoAsync(question, cancellationToken);
    }

    public async Task NotifyHealthIssueAsync(DiagnosticReport report, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (report.Status == HealthStatus.Healthy)
        {
            return;
        }

        var messageBuilder = new StringBuilder();
        messageBuilder.Append($"Status: {report.Status}. ");

        if (report.Findings.Count > 0)
        {
            messageBuilder.Append("Findings: ");
            messageBuilder.Append(string.Join(" | ", report.Findings.Take(3)));
        }

        if (report.SuggestedFixes.Count > 0)
        {
            messageBuilder.Append(" Suggested actions: ");
            messageBuilder.Append(string.Join(" | ", report.SuggestedFixes.Take(2)));
        }

        var content = new NotificationContent(
            Title: "WinAgent Health Alert",
            Message: messageBuilder.ToString(),
            ExpirationUtc: DateTimeOffset.UtcNow.AddMinutes(10),
            Type: report.Status == HealthStatus.Unhealthy ? NotificationType.Critical : NotificationType.Warning);

        await NotifyAsync(content, cancellationToken).ConfigureAwait(false);
    }

    public Task SetTrayStatusAsync(TrayStatus status, string? tooltip = null, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        _trayIconManager.SetStatus(status, tooltip);
        return Task.CompletedTask;
    }

    public Task PlayCompletionSoundAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (FocusAssistHelper.IsLikelyEnabled())
        {
            _logger.LogInformation("Focus assist appears enabled; skipping completion sound.");
            return Task.CompletedTask;
        }

        SystemSounds.Asterisk.Play();
        return Task.CompletedTask;
    }

    public Task PlayCriticalAlertSoundAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        SystemSounds.Hand.Play();
        return Task.CompletedTask;
    }

    public async ValueTask DisposeAsync()
    {
        await _trayIconManager.DisposeAsync().ConfigureAwait(false);
    }
}

internal static class FocusAssistHelper
{
    public static bool IsLikelyEnabled()
    {
        try
        {
            using var key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Notifications\Settings");
            var value = key?.GetValue("NOC_GLOBAL_SETTING_TOASTS_ENABLED");
            return value is int intValue && intValue == 0;
        }
        catch
        {
            return false;
        }
    }
}
