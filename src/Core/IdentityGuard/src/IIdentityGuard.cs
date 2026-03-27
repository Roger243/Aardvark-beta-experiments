using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.IdentityGuard;

public interface IIdentityGuard : IModule
{
    string Name => "IdentityGuard";
}
