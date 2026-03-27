using System.Threading;
using System.Threading.Tasks;

namespace SelfDiagnosticModule.Healing;

public interface IHealerAction
{
    string Name { get; }
    Task<bool> TryHealAsync(CancellationToken cancellationToken = default);
}
