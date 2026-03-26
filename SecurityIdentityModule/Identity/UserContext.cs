using System.Security.Principal;

namespace SecurityIdentityModule.Identity;

public sealed class UserContext
{
    public string UserName { get; }
    public string? UserSid { get; }
    public bool IsElevated { get; }

    public UserContext()
    {
        using var identity = WindowsIdentity.GetCurrent();
        UserName = identity?.Name ?? "Unknown";
        UserSid = identity?.User?.Value;

        using var principal = new WindowsPrincipal(identity!);
        IsElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
    }

    public bool MatchesExpectedUser(string? expectedUser)
        => string.IsNullOrWhiteSpace(expectedUser)
            || string.Equals(UserName, expectedUser, System.StringComparison.OrdinalIgnoreCase);
}
