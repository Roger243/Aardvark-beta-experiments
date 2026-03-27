using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using CommandExecutorModule.Models;
using CommandExecutorModule.Security;

namespace CommandExecutorModule.Executors;

public sealed record PowerShellExecutionResult(int ExitCode, string StandardOutput, string StandardError);

public sealed class PowerShellExecutor
{
    public async Task<PowerShellExecutionResult> ExecuteAsync(
        string script,
        ElevationContext? elevationContext = null,
        CancellationToken cancellationToken = default)
    {
        elevationContext ??= ElevationContext.Standard();

        var needsElevation = UacHelper.NeedsElevation(elevationContext);

        if (needsElevation)
        {
            if (!elevationContext.AllowUacPrompt)
            {
                if (elevationContext.StrictRequirement)
                {
                    throw new UnauthorizedAccessException("Elevation required but UAC prompting is disabled by policy.");
                }
            }
            else
            {
                if (elevationContext.RequiredLevel == ElevationLevel.System)
                {
                    throw new UnauthorizedAccessException(
                        "SYSTEM-level execution cannot be obtained via UAC runas alone; run from a trusted service context.");
                }

                var args = UacHelper.BuildPowerShellArgumentString(script, useEncodedCommand: true);
                var exitCode = await UacHelper.RelaunchWithRunAsAsync(
                    executable: "powershell.exe",
                    arguments: args,
                    timeout: elevationContext.Timeout,
                    cancellationToken: cancellationToken).ConfigureAwait(false);

                return new PowerShellExecutionResult(exitCode, string.Empty, string.Empty);
            }
        }

        // Execute in current token context.
        return await ExecuteInCurrentContextAsync(script, elevationContext.Timeout, cancellationToken).ConfigureAwait(false);
    }

    private static async Task<PowerShellExecutionResult> ExecuteInCurrentContextAsync(
        string script,
        TimeSpan? timeout,
        CancellationToken cancellationToken)
    {
        var psi = new ProcessStartInfo
        {
            FileName = "powershell.exe",
            Arguments = UacHelper.BuildPowerShellArgumentString(script, useEncodedCommand: true),
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            UseShellExecute = false,
            CreateNoWindow = true,
        };

        using var process = new Process { StartInfo = psi, EnableRaisingEvents = true };

        try
        {
            if (!process.Start())
            {
                throw new InvalidOperationException("Failed to start PowerShell process.");
            }
        }
        catch (Win32Exception ex) when (PrivilegeManager.IsAccessDenied(ex))
        {
            throw new AccessDeniedExecutionException("Access denied while starting PowerShell (0x5).", ex);
        }

        var stdOutTask = process.StandardOutput.ReadToEndAsync(cancellationToken).AsTask();
        var stdErrTask = process.StandardError.ReadToEndAsync(cancellationToken).AsTask();

        using var linkedCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        if (timeout.HasValue)
        {
            linkedCts.CancelAfter(timeout.Value);
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

            throw new TimeoutException("PowerShell command timed out.");
        }

        var stdOut = await stdOutTask.ConfigureAwait(false);
        var stdErr = await stdErrTask.ConfigureAwait(false);
        return new PowerShellExecutionResult(process.ExitCode, stdOut, stdErr);
    }
}
