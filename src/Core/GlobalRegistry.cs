using System.Collections.Generic;
using WinAgent.Core;
using WinAgent.Core.AgentHost;
using WinAgent.Core.ModuleRegistry;
using WinAgent.Core.HealthCoordinator;
using WinAgent.Core.PolicyEngine;
using WinAgent.Core.AuditBus;
using WinAgent.Core.ConfigCenter;
using WinAgent.Core.IdentityGuard;
using WinAgent.Core.TaskRouter;
using WinAgent.Core.TelemetryHub;
using WinAgent.Core.NotificationCenter;
using WinAgent.Core.DiagnosticsBridge;
using WinAgent.Core.IntegrationGateway;

namespace WinAgent;

public static class GlobalRegistry
{
    public static IReadOnlyList<IModule> CreateCoreModules()
        => new IModule[]
        {
            new AgentHost.AgentHost(),
            new ModuleRegistry.ModuleRegistry(),
            new HealthCoordinator.HealthCoordinator(),
            new PolicyEngine.PolicyEngine(),
            new AuditBus.AuditBus(),
            new ConfigCenter.ConfigCenter(),
            new IdentityGuard.IdentityGuard(),
            new TaskRouter.TaskRouter(),
            new TelemetryHub.TelemetryHub(),
            new NotificationCenter.NotificationCenter(),
            new DiagnosticsBridge.DiagnosticsBridge(),
            new IntegrationGateway.IntegrationGateway(),
        };

    public static IReadOnlyList<string> NativeModules { get; } =
        new[] { "KernelEventMonitor", "MemoryIntegrity", "StorageHealth", "HookAudit", "NetworkTelemetry", "GuardedCapture", "InputSafety", "FirmwareAudit", "DebugSignalAudit", "RuntimeIsolator" };

    public static IReadOnlyList<string> IntelligenceModules { get; } =
        new[] { "LlmConnector", "LogAnalyzer", "SafeInterpreter", "VisionGuide", "PredictiveHealth", "DataProcessor", "ScrapingExpert", "TrafficAnalyzer", "ForensicsTool", "BotBridge" };

    public static IReadOnlyList<string> WebModules { get; } =
        new[] { "AdminDash", "LogPipe", "FlowEditor", "AuthService", "ReportGen", "ApiGateway", "CloudSync", "BrowserAudit", "Notifier", "WebTerminal" };

    public static IReadOnlyList<string> LowLevelModules { get; } =
        new[] { "CpuProbe", "DataProtector", "CryptoAccel", "SysInfoInvoker", "VectorMath", "StackGuard", "ContextMgr", "JitOpt", "EntropySource", "StateAudit" };
}
