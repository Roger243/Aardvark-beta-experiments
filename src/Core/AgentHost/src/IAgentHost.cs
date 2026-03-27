using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.AgentHost;

public interface IAgentHost : IModule
{
    string Name => "AgentHost";
}
