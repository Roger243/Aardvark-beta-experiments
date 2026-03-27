using System.Collections.Generic;

namespace SecurityIdentityModule.Models;

public sealed record SecurityPolicy(
    string SecurityLevel,
    IReadOnlyList<string> AllowedCommandPatterns,
    IReadOnlyList<string> ForbiddenCommandPatterns,
    IReadOnlyList<string> AllowedPathPrefixes,
    IReadOnlyList<string> ForbiddenPathPrefixes);
