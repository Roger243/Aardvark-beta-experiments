using System;
using System.IO;
using System.Text.Json;
using SecurityIdentityModule.Models;

namespace SecurityIdentityModule.Policies;

public sealed class JsonPolicyProvider : IPolicyProvider
{
    private readonly string _policyPath;
    private readonly SecurityPolicy _fallback;

    public JsonPolicyProvider(string policyPath, SecurityPolicy? fallback = null)
    {
        _policyPath = policyPath;
        _fallback = fallback ?? new SecurityPolicy(
            "High",
            AllowedCommandPatterns: Array.Empty<string>(),
            ForbiddenCommandPatterns: new[] { "format", "del /s", "cipher /w", "diskpart clean" },
            AllowedPathPrefixes: Array.Empty<string>(),
            ForbiddenPathPrefixes: new[] { "C:\\Windows", "C:\\Program Files" });
    }

    public SecurityPolicy GetCurrentPolicy()
    {
        try
        {
            if (!File.Exists(_policyPath))
            {
                return _fallback;
            }

            var json = File.ReadAllText(_policyPath);
            return JsonSerializer.Deserialize<SecurityPolicy>(json) ?? _fallback;
        }
        catch
        {
            return _fallback;
        }
    }
}
