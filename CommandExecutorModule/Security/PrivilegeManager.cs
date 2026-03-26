using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Security.Principal;

namespace CommandExecutorModule.Security;

public static class PrivilegeManager
{
    [DllImport("shell32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool IsUserAnAdmin();

    public static bool IsAdministratorByPrincipal()
    {
        using var identity = WindowsIdentity.GetCurrent();
        var principal = new WindowsPrincipal(identity);
        return principal.IsInRole(WindowsBuiltInRole.Administrator);
    }

    public static bool IsAdministratorByShell32()
    {
        try
        {
            return IsUserAnAdmin();
        }
        catch (Exception ex) when (ex is DllNotFoundException or EntryPointNotFoundException)
        {
            throw new InvalidOperationException("Unable to evaluate admin state via shell32!IsUserAnAdmin.", ex);
        }
    }

    public static bool IsAdministrator()
    {
        // Prefer WindowsPrincipal for modern behavior; shell32 is provided as explicit requirement fallback.
        return IsAdministratorByPrincipal() || IsAdministratorByShell32();
    }

    public static bool IsSystemAccount()
    {
        using var identity = WindowsIdentity.GetCurrent();
        return identity.User != null && identity.User.IsWellKnown(WellKnownSidType.LocalSystemSid);
    }

    public static void EnsurePrivilegeLevel(bool requireAdmin, bool requireSystem)
    {
        if (requireSystem && !IsSystemAccount())
        {
            throw new UnauthorizedAccessException("Operation requires LocalSystem token context.");
        }

        if (requireAdmin && !IsAdministrator())
        {
            throw new UnauthorizedAccessException("Operation requires Administrator privileges.");
        }
    }

    public static bool IsAccessDenied(Win32Exception ex) => ex.NativeErrorCode == 0x5;
    public static bool IsUacCancelled(Win32Exception ex) => ex.NativeErrorCode == 1223;
}
