using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;

namespace SecurityIdentityModule.Audit;

public sealed class SecurityAuditLogger
{
    private readonly ILogger<SecurityAuditLogger> _logger;

    public SecurityAuditLogger(ILogger<SecurityAuditLogger> logger)
    {
        _logger = logger;
    }

    public Task WriteAsync(string eventName, string detail, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var safeDetail = detail?.Length > 1024 ? detail[..1024] : detail;
        _logger.LogWarning("SECURITY_AUDIT {Event} {Detail} {Utc}", eventName, safeDetail, DateTimeOffset.UtcNow);
        return Task.CompletedTask;
    }
}
