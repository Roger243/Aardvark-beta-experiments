using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

namespace NetworkConnectivityModule.Connectivity;

public sealed partial class WinApiConnectivity : IConnectivityTester
{
    [LibraryImport("wininet.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static partial bool InternetGetConnectedState(out int flags, int reserved);

    public Task<bool> IsConnectedAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var isConnected = InternetGetConnectedState(out _, 0);
        return Task.FromResult(isConnected);
    }
}
