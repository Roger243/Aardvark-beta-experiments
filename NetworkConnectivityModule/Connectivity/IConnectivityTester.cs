using System.Threading;
using System.Threading.Tasks;

namespace NetworkConnectivityModule.Connectivity;

public interface IConnectivityTester
{
    Task<bool> IsConnectedAsync(CancellationToken cancellationToken = default);
}
