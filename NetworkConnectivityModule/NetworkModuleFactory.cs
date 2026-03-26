using System;
using NetworkConnectivityModule.Connectivity;
using NetworkConnectivityModule.Http;
using NetworkConnectivityModule.Scanning;

namespace NetworkConnectivityModule;

public static class NetworkModuleFactory
{
    public static INetworkManager CreateDefault(
        TimeSpan? httpTimeout = null,
        int httpRetries = 3,
        string? userAgent = null)
    {
        IConnectivityTester connectivity = new WinApiConnectivity();
        IApiClient apiClient = new RobustHttpClient(timeout: httpTimeout, maxRetries: httpRetries, userAgent: userAgent);
        IPortScanner portScanner = new TcpPortScanner();

        return new NetworkService(connectivity, apiClient, portScanner);
    }
}
