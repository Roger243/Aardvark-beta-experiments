using System;
using System.Net;

namespace NetworkConnectivityModule.Utils;

public static class NetworkUtils
{
    public static bool IsValidHost(string host)
    {
        if (string.IsNullOrWhiteSpace(host))
        {
            return false;
        }

        return Uri.CheckHostName(host) != UriHostNameType.Unknown || IPAddress.TryParse(host, out _);
    }

    public static int NormalizePort(int port)
    {
        if (port is < 1 or > 65535)
        {
            throw new ArgumentOutOfRangeException(nameof(port), "Port must be between 1 and 65535.");
        }

        return port;
    }
}
