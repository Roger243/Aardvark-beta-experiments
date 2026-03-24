using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using FileIntelligenceModule.Operations;
using Microsoft.Extensions.Logging;
using SelfDiagnosticModule.Models;
using SelfDiagnosticModule.Storage;

namespace SelfDiagnosticModule.Checks;

public sealed class FileIntegrityCheck : IHealthCheck
{
    private readonly FileHasher _fileHasher;
    private readonly IntegrityManifest _manifest;
    private readonly string _mainExecutablePath;
    private readonly ILogger<FileIntegrityCheck> _logger;

    public FileIntegrityCheck(
        FileHasher fileHasher,
        IntegrityManifest manifest,
        string mainExecutablePath,
        ILogger<FileIntegrityCheck> logger)
    {
        _fileHasher = fileHasher;
        _manifest = manifest;
        _mainExecutablePath = mainExecutablePath;
        _logger = logger;
    }

    public string Name => nameof(FileIntegrityCheck);

    public async Task<DiagnosticReport> RunAsync(CancellationToken cancellationToken = default)
    {
        var findings = new List<string>();
        var fixes = new List<string>();

        try
        {
            if (!File.Exists(_mainExecutablePath))
            {
                findings.Add($"Missing executable: {_mainExecutablePath}");
                fixes.Add("Restore WinAgent.exe from a trusted package.");
                return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Unhealthy, findings, fixes);
            }

            if (!_manifest.TryGetExpectedHash(_mainExecutablePath, out var expectedHash) || string.IsNullOrWhiteSpace(expectedHash))
            {
                findings.Add("No expected hash available for executable; integrity check limited.");
                fixes.Add("Populate IntegrityManifest with the expected SHA256 hash.");
                return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Degraded, findings, fixes);
            }

            var computedHash = await _fileHasher.ComputeSha256Async(_mainExecutablePath, cancellationToken).ConfigureAwait(false);
            if (!string.Equals(computedHash, expectedHash, StringComparison.OrdinalIgnoreCase))
            {
                findings.Add("Executable hash mismatch detected.");
                fixes.Add("Replace executable with signed trusted build and re-run diagnostics.");
                return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Unhealthy, findings, fixes);
            }

            findings.Add("Executable hash verified.");
            return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Healthy, findings, fixes);
        }
        catch (OperationCanceledException)
        {
            findings.Add("File integrity check canceled.");
            return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Degraded, findings, fixes);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "File integrity check failed unexpectedly.");
            findings.Add($"Unexpected error during integrity check: {ex.Message}");
            fixes.Add("Retry check and inspect local logs for stack trace details.");
            return new DiagnosticReport(DateTimeOffset.UtcNow, HealthStatus.Degraded, findings, fixes);
        }
    }
}
