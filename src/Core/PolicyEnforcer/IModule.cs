namespace WinAgent.Core.PolicyEnforcer;

public interface IModule : WinAgent.Core.IModule
{
    string DiagnosticScope => "PolicyEnforcer";
}
