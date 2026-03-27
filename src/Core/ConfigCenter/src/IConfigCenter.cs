using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.ConfigCenter;

public interface IConfigCenter : IModule
{
    string Name => "ConfigCenter";
}
