using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace NetworkConnectivityModule.Scanning;

public interface IPortScanner
{
    Task<IReadOnlyList<int>> ScanOpenPortsAsync(string host, IEnumerable<int> ports, CancellationToken cancellationToken = default);
}
