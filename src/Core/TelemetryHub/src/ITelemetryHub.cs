using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.TelemetryHub;

public interface ITelemetryHub : IModule
{
    string Name => "TelemetryHub";
}
