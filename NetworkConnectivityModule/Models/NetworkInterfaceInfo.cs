using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;

namespace NetworkConnectivityModule.Models;

public sealed record NetworkInterfaceInfo(
    string Name,
    string Description,
    string Type,
    OperationalStatus Status,
    bool SupportsIpv4,
    bool SupportsIpv6,
    long SpeedBitsPerSecond,
    IReadOnlyList<string> UnicastAddresses)
{
    public static IReadOnlyList<NetworkInterfaceInfo> CaptureAll()
    {
        var interfaces = NetworkInterface.GetAllNetworkInterfaces();
        var results = new List<NetworkInterfaceInfo>(interfaces.Length);

        foreach (var nic in interfaces)
        {
            var ipProps = nic.GetIPProperties();
            var addresses = ipProps.UnicastAddresses
                .Select(static a => a.Address.ToString())
                .ToArray();

            results.Add(new NetworkInterfaceInfo(
                Name: nic.Name,
                Description: nic.Description,
                Type: nic.NetworkInterfaceType.ToString(),
                Status: nic.OperationalStatus,
                SupportsIpv4: nic.Supports(NetworkInterfaceComponent.IPv4),
                SupportsIpv6: nic.Supports(NetworkInterfaceComponent.IPv6),
                SpeedBitsPerSecond: nic.Speed,
                UnicastAddresses: addresses));
        }

        return results;
    }

    public static DateTimeOffset CapturedAtUtc() => DateTimeOffset.UtcNow;
}
