namespace WinAgent.Core.AmsiScanner;

public interface IModule : WinAgent.Core.IModule
{
    string DiagnosticScope => "AmsiScanner";
}
