using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.PolicyEngine;

public interface IPolicyEngine : IModule
{
    string Name => "PolicyEngine";
}
