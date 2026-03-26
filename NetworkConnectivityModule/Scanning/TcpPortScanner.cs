using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Threading;
using System.Threading.Tasks;
using NetworkConnectivityModule.Utils;

namespace NetworkConnectivityModule.Scanning;

public sealed class TcpPortScanner : IPortScanner
{
    private readonly TimeSpan _connectTimeout;

    public TcpPortScanner(TimeSpan? connectTimeout = null)
    {
        _connectTimeout = connectTimeout ?? TimeSpan.FromMilliseconds(700);
    }

    public async Task<IReadOnlyList<int>> ScanOpenPortsAsync(string host, IEnumerable<int> ports, CancellationToken cancellationToken = default)
    {
        if (!NetworkUtils.IsValidHost(host))
        {
            throw new ArgumentException("Host is invalid.", nameof(host));
        }

        var normalizedPorts = ports.Select(NetworkUtils.NormalizePort).Distinct().ToArray();
        var openPorts = new List<int>(normalizedPorts.Length);

        foreach (var port in normalizedPorts)
        {
            cancellationToken.ThrowIfCancellationRequested();
            if (await IsOpenAsync(host, port, cancellationToken).ConfigureAwait(false))
            {
                openPorts.Add(port);
            }
        }

        return openPorts;
    }

    private async Task<bool> IsOpenAsync(string host, int port, CancellationToken cancellationToken)
    {
        using var client = new TcpClient();
        using var timeoutCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        timeoutCts.CancelAfter(_connectTimeout);

        try
        {
            await client.ConnectAsync(host, port, timeoutCts.Token).ConfigureAwait(false);
            return true;
        }
        catch (OperationCanceledException) when (!cancellationToken.IsCancellationRequested)
        {
            return false;
        }
        catch (SocketException)
        {
            return false;
        }
    }
}
