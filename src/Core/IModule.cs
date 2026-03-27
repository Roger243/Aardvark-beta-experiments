using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core;

public interface IModule
{
    string Name { get; }
    Task InitializeAsync(CancellationToken cancellationToken = default);
}
