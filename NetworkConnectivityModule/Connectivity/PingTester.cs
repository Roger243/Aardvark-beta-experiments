using System.Net.NetworkInformation;
using System.Threading;
using System.Threading.Tasks;

namespace NetworkConnectivityModule.Connectivity;

public sealed class PingTester : IConnectivityTester
{
    private readonly string _host;
    private readonly int _timeoutMs;

    public PingTester(string host = "1.1.1.1", int timeoutMs = 1500)
    {
        _host = host;
        _timeoutMs = timeoutMs;
    }

    public async Task<bool> IsConnectedAsync(CancellationToken cancellationToken = default)
    {
        using var ping = new Ping();
        var response = await ping.SendPingAsync(_host, _timeoutMs).WaitAsync(cancellationToken).ConfigureAwait(false);
        return response.Status == IPStatus.Success;
    }
}
