using System.Net;

namespace NetworkConnectivityModule.Models;

public sealed record HttpResult(
    bool Success,
    HttpStatusCode StatusCode,
    string Content,
    string? ErrorMessage = null);
