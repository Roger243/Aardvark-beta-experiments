using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.NotificationCenter;

public interface INotificationCenter : IModule
{
    string Name => "NotificationCenter";
}
