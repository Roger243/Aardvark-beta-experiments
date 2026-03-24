using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using NetworkConnectivityModule.Connectivity;
using NetworkConnectivityModule.Events;
using NetworkConnectivityModule.Http;
using NetworkConnectivityModule.Models;
using NetworkConnectivityModule.Scanning;

namespace NetworkConnectivityModule;

public sealed class NetworkService : INetworkManager
{
    private readonly IConnectivityTester _connectivityTester;
    private readonly IApiClient _apiClient;
    private readonly IPortScanner _portScanner;
    private bool _lastKnownState;

    public NetworkService(IConnectivityTester connectivityTester, IApiClient apiClient, IPortScanner portScanner)
    {
        _connectivityTester = connectivityTester;
        _apiClient = apiClient;
        _portScanner = portScanner;
    }

    public event EventHandler<NetworkStateChangedEventArgs>? NetworkStateChanged;

    public async Task<bool> IsConnectedAsync(CancellationToken cancellationToken = default)
    {
        var state = await _connectivityTester.IsConnectedAsync(cancellationToken).ConfigureAwait(false);

        if (state != _lastKnownState)
        {
            _lastKnownState = state;
            NetworkStateChanged?.Invoke(this, new NetworkStateChangedEventArgs(state, DateTimeOffset.UtcNow));
        }

        return state;
    }

    public Task<IReadOnlyList<NetworkInterfaceInfo>> GetInterfacesAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        return Task.FromResult(NetworkInterfaceInfo.CaptureAll());
    }

    public Task<HttpResult> SendGetAsync(Uri requestUri, CancellationToken cancellationToken = default)
        => _apiClient.GetAsync(requestUri, cancellationToken);

    public Task<HttpResult> SendPostJsonAsync(Uri requestUri, string jsonPayload, CancellationToken cancellationToken = default)
        => _apiClient.PostJsonAsync(requestUri, jsonPayload, cancellationToken);

    public Task<IReadOnlyList<int>> ScanOpenPortsAsync(string host, IEnumerable<int> ports, CancellationToken cancellationToken = default)
        => _portScanner.ScanOpenPortsAsync(host, ports, cancellationToken);

    public ValueTask DisposeAsync()
    {
        if (_apiClient is IDisposable disposable)
        {
            disposable.Dispose();
        }

        return ValueTask.CompletedTask;
    }
}
