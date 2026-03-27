namespace WinAgent.Core.DynamicLoader;

public interface IModule : WinAgent.Core.IModule
{
    string DiagnosticScope => "DynamicLoader";
}
