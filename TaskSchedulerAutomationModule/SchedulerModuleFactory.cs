using TaskSchedulerAutomationModule.Tasks;

namespace TaskSchedulerAutomationModule;

public static class SchedulerModuleFactory
{
    public static ISchedulerManager CreateDefault()
    {
        ITaskProvider provider = new WinTaskProvider();
        return new SchedulerService(provider);
    }
}
