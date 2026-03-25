using Microsoft.Extensions.Logging.Abstractions;
using UserUiNotificationModule.Dialogs;
using UserUiNotificationModule.Toasts;
using UserUiNotificationModule.Tray;

namespace UserUiNotificationModule;

public static class UiModuleFactory
{
    public static IUiManager CreateDefault()
    {
        var toastProvider = new WinRtToastProvider();
        var dialogProvider = new NativeDialogProvider();
        var tray = new TrayIconManager();
        return new NotificationService(toastProvider, dialogProvider, tray, NullLogger<NotificationService>.Instance);
    }
}
