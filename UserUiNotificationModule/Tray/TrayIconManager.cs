using System;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows.Forms;
using UserUiNotificationModule.Models;

namespace UserUiNotificationModule.Tray;

public sealed class TrayIconManager : IAsyncDisposable
{
    private readonly NotifyIcon _notifyIcon;

    public TrayIconManager()
    {
        _notifyIcon = new NotifyIcon
        {
            Icon = SystemIcons.Application,
            Visible = true,
            Text = "WinAgent: Idle"
        };
    }

    public void SetStatus(TrayStatus status, string? tooltip = null)
    {
        _notifyIcon.Text = tooltip ?? $"WinAgent: {status}";
        _notifyIcon.Icon = status switch
        {
            TrayStatus.Warning => SystemIcons.Warning,
            TrayStatus.Error => SystemIcons.Error,
            TrayStatus.Processing => SystemIcons.Information,
            _ => SystemIcons.Application
        };
    }

    public ValueTask DisposeAsync()
    {
        _notifyIcon.Visible = false;
        _notifyIcon.Dispose();
        return ValueTask.CompletedTask;
    }
}
