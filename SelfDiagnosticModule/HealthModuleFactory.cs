using System;
using System.Collections.Generic;
using FileIntelligenceModule.Operations;
using Microsoft.Extensions.Logging;
using SelfDiagnosticModule.Checks;
using SelfDiagnosticModule.Healing;
using SelfDiagnosticModule.Storage;

namespace SelfDiagnosticModule;

public static class HealthModuleFactory
{
    public static IHealthManager CreateDefault(
        ILoggerFactory loggerFactory,
        string appBasePath,
        string settingsPath,
        string settingsTemplateJson,
        string? expectedExecutableSha256 = null)
    {
        var fileHasher = new FileHasher();
        var atomicWriter = new AtomicWriter();
        var manifest = IntegrityManifest.CreateDefault(appBasePath, expectedExecutableSha256);

        var checks = new List<IHealthCheck>
        {
            new FileIntegrityCheck(
                fileHasher,
                manifest,
                System.IO.Path.Combine(appBasePath, "WinAgent.exe"),
                loggerFactory.CreateLogger<FileIntegrityCheck>()),
            new SystemDependencyCheck(),
            new ModuleResponsivenessCheck(
                probes: new Dictionary<string, Func<System.Threading.CancellationToken, System.Threading.Tasks.Task<bool>>>
                {
                    ["Executor"] = _ => System.Threading.Tasks.Task.FromResult(true),
                    ["Monitor"] = _ => System.Threading.Tasks.Task.FromResult(true),
                    ["Network"] = _ => System.Threading.Tasks.Task.FromResult(true)
                },
                logger: loggerFactory.CreateLogger<ModuleResponsivenessCheck>())
        };

        var healers = new List<IHealerAction>
        {
            new ConfigRecoveryAction(
                atomicWriter,
                loggerFactory.CreateLogger<ConfigRecoveryAction>(),
                settingsPath,
                settingsTemplateJson)
        };

        return new DiagnosticService(checks, healers, loggerFactory.CreateLogger<DiagnosticService>());
    }
}
