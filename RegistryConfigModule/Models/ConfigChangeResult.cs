namespace RegistryConfigModule.Models;

public sealed record ConfigChangeResult(
    bool Success,
    string Message,
    string? BackupPath = null,
    string? ErrorCode = null)
{
    public static ConfigChangeResult Ok(string message, string? backupPath = null)
        => new(true, message, backupPath);

    public static ConfigChangeResult Fail(string message, string? errorCode = null, string? backupPath = null)
        => new(false, message, backupPath, errorCode);
}
