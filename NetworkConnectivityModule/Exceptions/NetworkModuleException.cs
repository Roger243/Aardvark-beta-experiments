using System;

namespace NetworkConnectivityModule.Exceptions;

public sealed class NetworkModuleException : Exception
{
    public NetworkModuleException(string message)
        : base(message)
    {
    }

    public NetworkModuleException(string message, Exception innerException)
        : base(message, innerException)
    {
    }
}
