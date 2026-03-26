using System.Security.Cryptography;
using Microsoft.Extensions.Logging;
using SecurityIdentityModule.Audit;
using SecurityIdentityModule.Identity;
using SecurityIdentityModule.Policies;
using SecurityIdentityModule.Vault;

namespace SecurityIdentityModule;

public static class SecurityModuleFactory
{
    public static ISecurityManager CreateDefault(string policyPath, ILoggerFactory loggerFactory)
    {
        ISecretVault vault = new DpapiVaultProvider(DataProtectionScope.CurrentUser);
        IPolicyProvider policies = new JsonPolicyProvider(policyPath);
        var commandGuard = new CommandGuard();
        var auditLogger = new SecurityAuditLogger(loggerFactory.CreateLogger<SecurityAuditLogger>());
        var userContext = new UserContext();

        return new SecurityService(vault, policies, commandGuard, auditLogger, userContext);
    }
}
