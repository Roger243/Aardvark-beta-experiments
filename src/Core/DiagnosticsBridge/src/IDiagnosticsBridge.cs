using System.Threading;
using System.Threading.Tasks;

namespace WinAgent.Core.DiagnosticsBridge;

public interface IDiagnosticsBridge : IModule
{
    string Name => "DiagnosticsBridge";
}
