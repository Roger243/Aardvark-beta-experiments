using System;
using System.Security;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;
using UserUiNotificationModule.Exceptions;

namespace UserUiNotificationModule.Toasts;

public sealed class WinRtToastProvider : IToastProvider
{
    private const string AppId = "WinAgent";

    public Task ShowToastAsync(string title, string message, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        try
        {
            var xml = $"""
                      <toast>
                        <visual>
                          <binding template='ToastGeneric'>
                            <text>{Escape(title)}</text>
                            <text>{Escape(message)}</text>
                          </binding>
                        </visual>
                      </toast>
                      """;

            var toastXml = new XmlDocument();
            toastXml.LoadXml(xml);

            var notification = new Windows.UI.Notifications.ToastNotification(toastXml);
            var notifier = Windows.UI.Notifications.ToastNotificationManager.CreateToastNotifier(AppId);
            notifier.Show(notification);

            return Task.CompletedTask;
        }
        catch (Exception ex)
        {
            throw new UiModuleException("Windows Toast API is unavailable or failed.", ex);
        }
    }

    private static string Escape(string value)
        => SecurityElement.Escape(value) ?? string.Empty;
}
