using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.IntegrationGateway;

public interface IIntegrationGateway : IModule
{
    string Name => "IntegrationGateway";
}
