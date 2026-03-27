using SystemMonitorModule.Models;
using SystemMonitorModule.Providers;
using Microsoft.Extensions.DependencyInjection;

namespace SystemMonitorModule;

public static class MonitorFactory
{
    public static IServiceCollection AddSystemMonitorModule(this IServiceCollection services, MonitorThresholds? thresholds = null)
    {
        services.AddSingleton(thresholds ?? new MonitorThresholds());
        services.AddSingleton<IMonitorProvider, WmiProvider>();
        services.AddSingleton<IMonitorProvider, EventLogProvider>();
        services.AddSingleton<IMonitorProvider, PerformanceCounterProvider>();
        services.AddSingleton<ISystemMonitor, MonitorService>();
        return services;
    }
}
