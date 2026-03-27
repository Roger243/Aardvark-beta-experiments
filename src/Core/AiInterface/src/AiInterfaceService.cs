using System;
using System.Diagnostics;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using WinAgent.Core;

namespace WinAgent.Core.AiInterface;

public sealed class AiInterfaceService : IModule, IAsyncDisposable
{
    private readonly HttpClient _httpClient;
    private readonly Uri _llmConnectorEndpoint;
    private readonly string _tauriExecutablePath;
    private readonly TimeSpan _contextSamplingInterval;

    private Process? _aiProcess;
    private CancellationTokenSource? _contextLoopCts;
    private Task? _contextLoop;

    public string Name => "AiInterfaceService";

    public AiInterfaceService()
        : this(new Uri("http://127.0.0.1:8765/v1/context"), "WinAgent.ChatUI.exe")
    {
    }

    public AiInterfaceService(
        Uri llmConnectorEndpoint,
        string tauriExecutablePath,
        HttpClient? httpClient = null,
        TimeSpan? contextSamplingInterval = null)
    {
        _llmConnectorEndpoint = llmConnectorEndpoint;
        _tauriExecutablePath = tauriExecutablePath;
        _httpClient = httpClient ?? new HttpClient();
        _contextSamplingInterval = contextSamplingInterval ?? TimeSpan.FromSeconds(2);
    }

    public async Task InitializeAsync(CancellationToken cancellationToken = default)
    {
        await StartAiWindowAsync(cancellationToken).ConfigureAwait(false);
        StartContextLoop(cancellationToken);
    }

    public async Task StartAiWindowAsync(CancellationToken cancellationToken = default)
    {
        if (_aiProcess is { HasExited: false })
        {
            return;
        }

        var startInfo = new ProcessStartInfo
        {
            FileName = _tauriExecutablePath,
            UseShellExecute = false,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            CreateNoWindow = false,
            Arguments = "--always-on-top --winagent-overlay"
        };

        _aiProcess = Process.Start(startInfo)
            ?? throw new InvalidOperationException("Failed to start AI interface process.");

        await Task.Delay(250, cancellationToken).ConfigureAwait(false);
    }

    public async Task StopAiWindowAsync(CancellationToken cancellationToken = default)
    {
        _contextLoopCts?.Cancel();

        if (_contextLoop is not null)
        {
            try { await _contextLoop.ConfigureAwait(false); }
            catch (OperationCanceledException) { }
        }

        if (_aiProcess is { HasExited: false })
        {
            _aiProcess.CloseMainWindow();

            if (!await WaitForExitAsync(_aiProcess, TimeSpan.FromSeconds(3), cancellationToken).ConfigureAwait(false))
            {
                _aiProcess.Kill(entireProcessTree: true);
            }
        }

        _aiProcess?.Dispose();
        _aiProcess = null;
    }

    public async Task PushTelemetryAsync(object telemetryPayload, CancellationToken cancellationToken = default)
    {
        var payload = JsonSerializer.Serialize(new
        {
            source = Name,
            timestampUtc = DateTimeOffset.UtcNow,
            telemetry = telemetryPayload,
            windowContext = CaptureActiveWindowContext()
        });

        using var request = new HttpRequestMessage(HttpMethod.Post, _llmConnectorEndpoint)
        {
            Content = new StringContent(payload, Encoding.UTF8, "application/json")
        };

        using var response = await _httpClient.SendAsync(request, cancellationToken).ConfigureAwait(false);
        response.EnsureSuccessStatusCode();
    }

    private void StartContextLoop(CancellationToken initToken)
    {
        _contextLoopCts = CancellationTokenSource.CreateLinkedTokenSource(initToken);
        _contextLoop = Task.Run(async () =>
        {
            while (!_contextLoopCts.IsCancellationRequested)
            {
                var context = CaptureActiveWindowContext();
                await PushTelemetryAsync(new { context }, _contextLoopCts.Token).ConfigureAwait(false);
                await Task.Delay(_contextSamplingInterval, _contextLoopCts.Token).ConfigureAwait(false);
            }
        }, _contextLoopCts.Token);
    }

    private static object CaptureActiveWindowContext()
    {
        var handle = GetForegroundWindow();
        if (handle == IntPtr.Zero)
        {
            return new { available = false };
        }

        _ = GetWindowThreadProcessId(handle, out var pid);
        var title = GetWindowText(handle);

        var processName = string.Empty;
        try
        {
            processName = Process.GetProcessById((int)pid).ProcessName;
        }
        catch
        {
            processName = "unknown";
        }

        return new
        {
            available = true,
            windowTitle = title,
            processId = pid,
            processName
        };
    }

    private static string GetWindowText(IntPtr handle)
    {
        var length = GetWindowTextLength(handle);
        var sb = new StringBuilder(length + 1);
        _ = GetWindowText(handle, sb, sb.Capacity);
        return sb.ToString();
    }

    private static async Task<bool> WaitForExitAsync(Process process, TimeSpan timeout, CancellationToken cancellationToken)
    {
        using var timeoutCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        timeoutCts.CancelAfter(timeout);

        try
        {
            await process.WaitForExitAsync(timeoutCts.Token).ConfigureAwait(false);
            return true;
        }
        catch (OperationCanceledException)
        {
            return false;
        }
    }

    public async ValueTask DisposeAsync()
    {
        await StopAiWindowAsync().ConfigureAwait(false);
        _httpClient.Dispose();
    }

    [DllImport("user32.dll")]
    private static extern IntPtr GetForegroundWindow();

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetWindowText(IntPtr hWnd, StringBuilder text, int count);

    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetWindowTextLength(IntPtr hWnd);

    [DllImport("user32.dll")]
    private static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint processId);
}
