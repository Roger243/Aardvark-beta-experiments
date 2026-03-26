namespace UserUiNotificationModule.Models;

public sealed record DialogRequest(
    string Prompt,
    string? DefaultValue = null,
    string ConfirmLabel = "OK",
    string CancelLabel = "Cancel");
