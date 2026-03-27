using System;
using System.Linq;
using SecurityIdentityModule.Models;

namespace SecurityIdentityModule.Policies;

public sealed class CommandGuard
{
    private static readonly string[] DefaultForbidden =
    {
        "format",
        "del /s",
        "del /q /s",
        "cipher /w",
        "diskpart clean",
        "bcdedit /delete",
        "reg delete hkcr"
    };

    public bool IsAllowed(string? command, SecurityPolicy policy, out string? reason)
    {
        reason = null;

        if (string.IsNullOrWhiteSpace(command))
        {
            reason = "Command is empty.";
            return false;
        }

        var normalized = command.Trim().ToLowerInvariant();
        var forbidden = (policy.ForbiddenCommandPatterns ?? Array.Empty<string>())
            .Concat(DefaultForbidden)
            .Where(s => !string.IsNullOrWhiteSpace(s))
            .Distinct(StringComparer.OrdinalIgnoreCase);

        foreach (var token in forbidden)
        {
            if (normalized.Contains(token.Trim().ToLowerInvariant(), StringComparison.Ordinal))
            {
                reason = $"Command blocked by forbidden keyword pattern: '{token}'.";
                return false;
            }
        }

        return true;
    }
}
