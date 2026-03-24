using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using NetworkConnectivityModule.Events;
using NetworkConnectivityModule.Models;

namespace NetworkConnectivityModule;

public interface INetworkManager : IAsyncDisposable
{
    event EventHandler<NetworkStateChangedEventArgs>? NetworkStateChanged;

    Task<bool> IsConnectedAsync(CancellationToken cancellationToken = default);
    Task<IReadOnlyList<NetworkInterfaceInfo>> GetInterfacesAsync(CancellationToken cancellationToken = default);
    Task<HttpResult> SendGetAsync(Uri requestUri, CancellationToken cancellationToken = default);
    Task<HttpResult> SendPostJsonAsync(Uri requestUri, string jsonPayload, CancellationToken cancellationToken = default);
    Task<IReadOnlyList<int>> ScanOpenPortsAsync(string host, IEnumerable<int> ports, CancellationToken cancellationToken = default);
}
