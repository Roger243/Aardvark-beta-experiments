using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.TaskRouter;

public interface ITaskRouter : IModule
{
    string Name => "TaskRouter";
}
