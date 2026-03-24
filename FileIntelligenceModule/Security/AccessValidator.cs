using System;
using System.IO;
using System.Security.AccessControl;
using System.Security.Principal;
using System.Threading;
using System.Threading.Tasks;

namespace FileIntelligenceModule.Security;

public sealed class AccessValidator
{
    public Task<bool> CanWriteAsync(string path, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        var normalized = PathUtil.NormalizeLongPath(path);
        var target = File.Exists(normalized) ? normalized : Path.GetDirectoryName(normalized) ?? normalized;
        if (!File.Exists(target) && !Directory.Exists(target))
        {
            return Task.FromResult(false);
        }

        try
        {
            if (File.Exists(target))
            {
                var attrs = File.GetAttributes(target);
                if ((attrs & FileAttributes.ReadOnly) == FileAttributes.ReadOnly)
                {
                    return Task.FromResult(false);
                }
            }

            var security = File.Exists(target)
                ? File.GetAccessControl(target)
                : Directory.GetAccessControl(target);

            var identity = WindowsIdentity.GetCurrent();
            var principal = new WindowsPrincipal(identity);

            foreach (FileSystemAccessRule rule in security.GetAccessRules(true, true, typeof(SecurityIdentifier)))
            {
                if (!identity.User!.Equals(rule.IdentityReference) && !principal.IsInRole((SecurityIdentifier)rule.IdentityReference))
                {
                    continue;
                }

                if ((rule.FileSystemRights & FileSystemRights.WriteData) != 0)
                {
                    return Task.FromResult(rule.AccessControlType == AccessControlType.Allow);
                }
            }
        }
        catch
        {
            return Task.FromResult(false);
        }

        return Task.FromResult(false);
    }
}
