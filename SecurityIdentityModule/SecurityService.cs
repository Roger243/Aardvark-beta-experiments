using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using ContextMemoryManager.History;
using SecurityIdentityModule.Audit;
using SecurityIdentityModule.Identity;
using SecurityIdentityModule.Models;
using SecurityIdentityModule.Policies;
using SecurityIdentityModule.Vault;

namespace SecurityIdentityModule;

public sealed class SecurityService : ISecurityManager
{
    private readonly ISecretVault _vault;
    private readonly IPolicyProvider _policyProvider;
    private readonly CommandGuard _commandGuard;
    private readonly SecurityAuditLogger _auditLogger;
    private readonly UserContext _userContext;

    public SecurityService(
        ISecretVault vault,
        IPolicyProvider policyProvider,
        CommandGuard commandGuard,
        SecurityAuditLogger auditLogger,
        UserContext userContext)
    {
        _vault = vault;
        _policyProvider = policyProvider;
        _commandGuard = commandGuard;
        _auditLogger = auditLogger;
        _userContext = userContext;
    }

    public async Task<bool> AuthorizeAsync(AccessRequest request, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var policy = _policyProvider.GetCurrentPolicy();

        if (!_commandGuard.IsAllowed(request.CommandText, policy, out var commandReason))
        {
            await _auditLogger.WriteAsync("CommandDenied", commandReason ?? "Denied by command guard.", cancellationToken).ConfigureAwait(false);
            return false;
        }

        if (!IsPathAllowed(request.TargetPath, policy, out var pathReason))
        {
            await _auditLogger.WriteAsync("PathDenied", pathReason ?? "Denied by path guard.", cancellationToken).ConfigureAwait(false);
            return false;
        }

        return true;
    }

    public async Task StoreSecretAsync(string key, string value, CancellationToken cancellationToken = default)
    {
        await _vault.ProtectAsync(key, value, cancellationToken).ConfigureAwait(false);
        await _auditLogger.WriteAsync("SecretStored", $"Secret stored under key '{key}'.", cancellationToken).ConfigureAwait(false);
    }

    public Task<string?> RetrieveSecretAsync(string key, CancellationToken cancellationToken = default)
        => _vault.UnprotectAsync(key, cancellationToken);

    public Task<bool> ValidateSessionAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        return Task.FromResult(_userContext.MatchesExpectedUser(Environment.UserName));
    }

    public async Task<bool> ValidateCommandAsync(string command, CancellationToken cancellationToken = default)
    {
        var policy = _policyProvider.GetCurrentPolicy();
        var isAllowed = _commandGuard.IsAllowed(command, policy, out var reason);

        if (!isAllowed)
        {
            await _auditLogger.WriteAsync("CommandValidationFailed", reason ?? "Forbidden command.", cancellationToken).ConfigureAwait(false);
        }

        return isAllowed;
    }

    public async Task LogSecurityEventAsync(string eventName, string detail, ExecutionLogEntry? sourceEntry = null, CancellationToken cancellationToken = default)
    {
        var safeDetail = sourceEntry is null
            ? detail
            : $"{detail} | Command={sourceEntry.CommandText} | Succeeded={sourceEntry.Succeeded}";

        await _auditLogger.WriteAsync(eventName, safeDetail, cancellationToken).ConfigureAwait(false);
    }

    private static bool IsPathAllowed(string? path, SecurityPolicy policy, out string? reason)
    {
        reason = null;
        if (string.IsNullOrWhiteSpace(path))
        {
            return true;
        }

        var fullPath = Path.GetFullPath(path);
        foreach (var forbiddenPrefix in policy.ForbiddenPathPrefixes)
        {
            if (fullPath.StartsWith(forbiddenPrefix, StringComparison.OrdinalIgnoreCase))
            {
                reason = $"Path '{fullPath}' blocked by forbidden prefix '{forbiddenPrefix}'.";
                return false;
            }
        }

        if (policy.AllowedPathPrefixes.Count == 0)
        {
            return true;
        }

        foreach (var allowedPrefix in policy.AllowedPathPrefixes)
        {
            if (fullPath.StartsWith(allowedPrefix, StringComparison.OrdinalIgnoreCase))
            {
                return true;
            }
        }

        reason = $"Path '{fullPath}' is not in the allowed prefix list.";
        return false;
    }
}
