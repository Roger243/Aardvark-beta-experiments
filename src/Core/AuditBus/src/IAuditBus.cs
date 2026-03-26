using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.AuditBus;

public interface IAuditBus : IModule
{
    string Name => "AuditBus";
}
