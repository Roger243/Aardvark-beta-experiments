using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using CommandExecutorModule.Models;

namespace CommandExecutorModule.Security;

public sealed class UacCancelledException : Exception
{
    public UacCancelledException(string message, Exception? inner = null) : base(message, inner) { }
}

public sealed class AccessDeniedExecutionException : Exception
{
    public AccessDeniedExecutionException(string message, Exception? inner = null) : base(message, inner) { }
}

public static class UacHelper
{
    public static bool NeedsElevation(ElevationContext context)
    {
        return context.RequiredLevel switch
        {
            ElevationLevel.Standard => false,
            ElevationLevel.Elevated => !PrivilegeManager.IsAdministrator(),
            ElevationLevel.System => !PrivilegeManager.IsSystemAccount(),
            _ => false
        };
    }

    public static async Task<int> RelaunchWithRunAsAsync(
        string executable,
        string arguments,
        TimeSpan? timeout = null,
        CancellationToken cancellationToken = default)
    {
        var startInfo = new ProcessStartInfo
        {
            FileName = executable,
            Arguments = arguments,
            UseShellExecute = true,
            Verb = "runas",
            CreateNoWindow = true,
            WindowStyle = ProcessWindowStyle.Hidden
        };

        Process? process;
        try
        {
            process = Process.Start(startInfo);
        }
        catch (Win32Exception ex) when (PrivilegeManager.IsUacCancelled(ex))
        {
            throw new UacCancelledException("User cancelled the UAC consent prompt.", ex);
        }
        catch (Win32Exception ex) when (PrivilegeManager.IsAccessDenied(ex))
        {
            throw new AccessDeniedExecutionException("Access denied while attempting elevated launch (0x5).", ex);
        }

        if (process is null)
        {
            throw new InvalidOperationException("Failed to start elevated process.");
        }

        var effectiveTimeout = timeout ?? Timeout.InfiniteTimeSpan;
        using var linkedCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        if (effectiveTimeout != Timeout.InfiniteTimeSpan)
        {
            linkedCts.CancelAfter(effectiveTimeout);
        }

        try
        {
            await process.WaitForExitAsync(linkedCts.Token).ConfigureAwait(false);
        }
        catch (OperationCanceledException) when (!cancellationToken.IsCancellationRequested)
        {
            try
            {
                if (!process.HasExited)
                {
                    process.Kill(entireProcessTree: true);
                }
            }
            catch
            {
                // Best effort cleanup.
            }

            throw new TimeoutException("Elevated process timed out.");
        }

        return process.ExitCode;
    }

    public static string BuildPowerShellArgumentString(string script, bool useEncodedCommand = true)
    {
        if (!useEncodedCommand)
        {
            return $"-NoProfile -NonInteractive -ExecutionPolicy Bypass -Command \"{script.Replace("\"", "\\\"")}\"";
        }

        var scriptBytes = Encoding.Unicode.GetBytes(script);
        var encoded = Convert.ToBase64String(scriptBytes);
        return $"-NoProfile -NonInteractive -ExecutionPolicy Bypass -EncodedCommand {encoded}";
    }
}
