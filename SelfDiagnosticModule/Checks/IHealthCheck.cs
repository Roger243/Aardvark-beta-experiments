using System.Threading;
using System.Threading.Tasks;
using SelfDiagnosticModule.Models;

namespace SelfDiagnosticModule.Checks;

public interface IHealthCheck
{
    string Name { get; }
    Task<DiagnosticReport> RunAsync(CancellationToken cancellationToken = default);
}
