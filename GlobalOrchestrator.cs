using System;
using System.Diagnostics;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WinAgent;

public sealed class GlobalOrchestrator : IAsyncDisposable
{
    private readonly string _pythonExecutable;
    private readonly string _pythonEntryPoint;
    private Process? _pythonProcess;

    public GlobalOrchestrator(string pythonExecutable, string pythonEntryPoint)
    {
        _pythonExecutable = pythonExecutable;
        _pythonEntryPoint = pythonEntryPoint;
    }

    [DllImport("CpuFeatureProbe", EntryPoint = "winagent_cpu_feature_mask", CallingConvention = CallingConvention.Cdecl)]
    private static extern uint GetCpuFeatureMask();

    public uint QueryCpuCapabilities()
    {
        try
        {
            return GetCpuFeatureMask();
        }
        catch (DllNotFoundException)
        {
            return 0;
        }
        catch (EntryPointNotFoundException)
        {
            return 0;
        }
    }

    public async Task<string> QueryJavaScriptGatewayAsync(string host, int port, string payload, CancellationToken cancellationToken = default)
    {
        using var client = new TcpClient();
        await client.ConnectAsync(host, port, cancellationToken).ConfigureAwait(false);

        await using var stream = client.GetStream();
        var bytes = Encoding.UTF8.GetBytes(payload + "\n");
        await stream.WriteAsync(bytes, cancellationToken).ConfigureAwait(false);

        var buffer = new byte[4096];
        var read = await stream.ReadAsync(buffer, cancellationToken).ConfigureAwait(false);
        return Encoding.UTF8.GetString(buffer, 0, read);
    }

    public Task StartPythonAnalyticsAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (_pythonProcess is { HasExited: false })
        {
            return Task.CompletedTask;
        }

        var psi = new ProcessStartInfo
        {
            FileName = _pythonExecutable,
            Arguments = _pythonEntryPoint,
            UseShellExecute = false,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            CreateNoWindow = true
        };

        _pythonProcess = Process.Start(psi);
        return Task.CompletedTask;
    }

    public async ValueTask DisposeAsync()
    {
        if (_pythonProcess is null)
        {
            return;
        }

        if (!_pythonProcess.HasExited)
        {
            _pythonProcess.Kill(entireProcessTree: true);
            await _pythonProcess.WaitForExitAsync().ConfigureAwait(false);
        }

        _pythonProcess.Dispose();
        _pythonProcess = null;
    }
}
