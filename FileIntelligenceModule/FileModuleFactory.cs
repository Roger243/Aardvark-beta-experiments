using Microsoft.Extensions.DependencyInjection;
using FileIntelligenceModule.Operations;
using FileIntelligenceModule.Search;
using FileIntelligenceModule.Security;

namespace FileIntelligenceModule;

public static class FileModuleFactory
{
    public static IServiceCollection AddFileIntelligenceModule(this IServiceCollection services)
    {
        services.AddSingleton<FileHasher>();
        services.AddSingleton<AccessValidator>();
        services.AddSingleton<AtomicWriter>();
        services.AddSingleton<FileSearcher>();
        services.AddSingleton<IFileManager, FileService>();
        return services;
    }
}
