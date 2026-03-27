namespace SecurityIdentityModule.Models;

public sealed record AccessRequest(
    string Subject,
    string Action,
    string? CommandText = null,
    string? TargetPath = null,
    string? RegistryPath = null);
