using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ModuleRegistry;

public interface IModuleRegistry : IModule
{
    string Name => "ModuleRegistry";
}
