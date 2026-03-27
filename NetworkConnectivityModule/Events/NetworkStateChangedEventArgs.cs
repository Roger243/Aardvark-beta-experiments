using System;

namespace NetworkConnectivityModule.Events;

public sealed class NetworkStateChangedEventArgs : EventArgs
{
    public NetworkStateChangedEventArgs(bool isConnected, DateTimeOffset observedAtUtc)
    {
        IsConnected = isConnected;
        ObservedAtUtc = observedAtUtc;
    }

    public bool IsConnected { get; }
    public DateTimeOffset ObservedAtUtc { get; }
}
