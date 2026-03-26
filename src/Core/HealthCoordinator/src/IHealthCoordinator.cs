using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.HealthCoordinator;

public interface IHealthCoordinator : IModule
{
    string Name => "HealthCoordinator";
}
