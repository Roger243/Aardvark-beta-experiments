using System.Threading;
using System.Threading.Tasks;
using SelfDiagnosticModule.Models;

namespace SelfDiagnosticModule;

public interface IHealthManager
{
    Task<DiagnosticReport> RunFullDiagnosticAsync(CancellationToken cancellationToken = default);
    Task<DiagnosticReport> RunQuickDiagnosticAsync(CancellationToken cancellationToken = default);
    Task StartHeartbeatAsync(CancellationToken cancellationToken = default);
    Task StopHeartbeatAsync(CancellationToken cancellationToken = default);
}
