using System;
using System.Diagnostics.Eventing.Reader;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using SystemMonitorModule.Events;
using SystemMonitorModule.Exceptions;
using SystemMonitorModule.Models;

namespace SystemMonitorModule.Providers;

public sealed class EventLogProvider : IMonitorProvider
{
    private readonly ILogger<EventLogProvider> _logger;
    private EventLogWatcher? _systemWatcher;
    private EventLogWatcher? _securityWatcher;
    private EventLogWatcher? _applicationWatcher;

    public string Name => "EventLog";

    public event EventHandler<MonitorEventArgs>? ProviderError;
    public event EventHandler<MonitorEventArgs>? CriticalEventReceived;

    public EventLogProvider(ILogger<EventLogProvider> logger)
    {
        _logger = logger;
    }

    public Task StartAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            _systemWatcher = BuildWatcher("System");
            _securityWatcher = BuildWatcher("Security");
            _applicationWatcher = BuildWatcher("Application");

            _systemWatcher.Enabled = true;
            _securityWatcher.Enabled = true;
            _applicationWatcher.Enabled = true;

            _logger.LogInformation("{Provider} started watching System/Security/Application logs", Name);
            return Task.CompletedTask;
        }
        catch (Exception ex)
        {
            var wrapped = new MonitorException("Failed to initialize EventLog watchers.", ex);
            _logger.LogError(wrapped, "{Provider} startup error", Name);
            ProviderError?.Invoke(this, MonitorEventArgs.FromError(Name, wrapped));
            throw wrapped;
        }
    }

    public Task StopAsync(CancellationToken cancellationToken = default)
    {
        DisposeWatchers();
        _logger.LogInformation("{Provider} stopped", Name);
        return Task.CompletedTask;
    }

    public Task<SystemMetrics> CollectAsync(CancellationToken cancellationToken = default)
    {
        // EventLogProvider is event-driven; expose lightweight heartbeat metrics.
        return Task.FromResult(new SystemMetrics
        {
            Source = Name,
            TimestampUtc = DateTimeOffset.UtcNow,
            CpuUsagePercent = 0,
            MemoryUsedMb = 0,
            MemoryTotalMb = 0
        });
    }

    public ValueTask DisposeAsync()
    {
        DisposeWatchers();
        return ValueTask.CompletedTask;
    }

    private EventLogWatcher BuildWatcher(string channel)
    {
        // Level 1 = Critical, Level 2 = Error
        const string xPath = "*[System[(Level=1 or Level=2)]]";
        var query = new EventLogQuery(channel, PathType.LogName, xPath)
        {
            TolerateQueryErrors = true,
            ReverseDirection = false
        };

        var watcher = new EventLogWatcher(query);
        watcher.EventRecordWritten += (_, args) => HandleEventRecord(channel, args);
        return watcher;
    }

    private void HandleEventRecord(string channel, EventRecordWrittenEventArgs args)
    {
        try
        {
            if (args.EventException is not null)
            {
                throw args.EventException;
            }

            if (args.EventRecord is null)
            {
                return;
            }

            using var record = args.EventRecord;

            var log = new LogEntry
            {
                Channel = channel,
                EventId = record.Id,
                Level = record.LevelDisplayName ?? record.Level?.ToString() ?? "Unknown",
                Provider = record.ProviderName ?? "Unknown",
                MachineName = record.MachineName ?? Environment.MachineName,
                Message = SafeFormatDescription(record),
                TimestampUtc = record.TimeCreated.HasValue
                    ? new DateTimeOffset(record.TimeCreated.Value.ToUniversalTime())
                    : DateTimeOffset.UtcNow
            };

            _logger.LogWarning("Critical/Error event observed: {Channel} {EventId} {Provider}", log.Channel, log.EventId, log.Provider);
            CriticalEventReceived?.Invoke(this, MonitorEventArgs.FromLog(log));
        }
        catch (Exception ex)
        {
            var wrapped = new MonitorException("Failed while processing Event Log record.", ex);
            _logger.LogError(wrapped, "{Provider} event processing error", Name);
            ProviderError?.Invoke(this, MonitorEventArgs.FromError(Name, wrapped));
        }
    }

    private static string SafeFormatDescription(EventRecord record)
    {
        try
        {
            return record.FormatDescription() ?? string.Empty;
        }
        catch
        {
            return string.Empty;
        }
    }

    private void DisposeWatchers()
    {
        _systemWatcher?.Dispose();
        _securityWatcher?.Dispose();
        _applicationWatcher?.Dispose();

        _systemWatcher = null;
        _securityWatcher = null;
        _applicationWatcher = null;
    }
}
