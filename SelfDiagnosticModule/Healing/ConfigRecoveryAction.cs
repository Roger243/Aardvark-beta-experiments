using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using FileIntelligenceModule.Operations;
using Microsoft.Extensions.Logging;

namespace SelfDiagnosticModule.Healing;

public sealed class ConfigRecoveryAction : IHealerAction
{
    private readonly AtomicWriter _atomicWriter;
    private readonly ILogger<ConfigRecoveryAction> _logger;
    private readonly string _settingsPath;
    private readonly string _templateJson;

    public ConfigRecoveryAction(
        AtomicWriter atomicWriter,
        ILogger<ConfigRecoveryAction> logger,
        string settingsPath,
        string templateJson)
    {
        _atomicWriter = atomicWriter;
        _logger = logger;
        _settingsPath = settingsPath;
        _templateJson = templateJson;
    }

    public string Name => nameof(ConfigRecoveryAction);

    public async Task<bool> TryHealAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            if (File.Exists(_settingsPath))
            {
                var current = await File.ReadAllTextAsync(_settingsPath, cancellationToken).ConfigureAwait(false);
                if (!string.IsNullOrWhiteSpace(current))
                {
                    _logger.LogInformation("Config recovery skipped; settings file already exists at {Path}", _settingsPath);
                    return true;
                }
            }

            var directory = Path.GetDirectoryName(_settingsPath);
            if (!string.IsNullOrWhiteSpace(directory))
            {
                Directory.CreateDirectory(directory);
            }

            await _atomicWriter.WriteAllTextAsync(_settingsPath, _templateJson, cancellationToken).ConfigureAwait(false);
            _logger.LogWarning("Recovered missing/corrupt settings file at {Path}", _settingsPath);
            return true;
        }
        catch (OperationCanceledException)
        {
            _logger.LogWarning("Config recovery canceled for {Path}", _settingsPath);
            return false;
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Failed to recover settings file at {Path}", _settingsPath);
            return false;
        }
    }
}
