using System;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using NetworkConnectivityModule.Models;

namespace NetworkConnectivityModule.Http;

public sealed class RobustHttpClient : IApiClient, IDisposable
{
    private readonly HttpClient _httpClient;
    private readonly int _maxRetries;

    public RobustHttpClient(HttpMessageHandler? handler = null, TimeSpan? timeout = null, int maxRetries = 3, string? userAgent = null)
    {
        _httpClient = handler is null ? new HttpClient() : new HttpClient(handler, disposeHandler: true);
        _httpClient.Timeout = timeout ?? TimeSpan.FromSeconds(10);
        _maxRetries = Math.Max(1, maxRetries);

        var resolvedAgent = string.IsNullOrWhiteSpace(userAgent)
            ? "WinAgent-NetworkConnectivityModule/1.0"
            : userAgent;

        _httpClient.DefaultRequestHeaders.UserAgent.ParseAdd(resolvedAgent);
    }

    public Task<HttpResult> GetAsync(Uri requestUri, CancellationToken cancellationToken = default)
        => SendWithRetryAsync(
            () => new HttpRequestMessage(HttpMethod.Get, requestUri),
            cancellationToken);

    public Task<HttpResult> PostJsonAsync(Uri requestUri, string jsonPayload, CancellationToken cancellationToken = default)
        => SendWithRetryAsync(
            () => new HttpRequestMessage(HttpMethod.Post, requestUri)
            {
                Content = new StringContent(jsonPayload, Encoding.UTF8, "application/json")
            },
            cancellationToken);

    private async Task<HttpResult> SendWithRetryAsync(
        Func<HttpRequestMessage> requestFactory,
        CancellationToken cancellationToken)
    {
        Exception? lastException = null;

        for (var attempt = 1; attempt <= _maxRetries; attempt++)
        {
            cancellationToken.ThrowIfCancellationRequested();

            try
            {
                using var request = requestFactory();
                using var response = await _httpClient.SendAsync(request, HttpCompletionOption.ResponseHeadersRead, cancellationToken).ConfigureAwait(false);
                var content = await response.Content.ReadAsStringAsync(cancellationToken).ConfigureAwait(false);

                if (response.IsSuccessStatusCode)
                {
                    return new HttpResult(true, response.StatusCode, content);
                }

                if (!IsTransientStatusCode(response.StatusCode) || attempt == _maxRetries)
                {
                    return new HttpResult(false, response.StatusCode, content, $"HTTP {(int)response.StatusCode}");
                }
            }
            catch (Exception ex) when (ex is HttpRequestException or TaskCanceledException)
            {
                lastException = ex;
                if (attempt == _maxRetries)
                {
                    break;
                }
            }

            await Task.Delay(ComputeBackoff(attempt), cancellationToken).ConfigureAwait(false);
        }

        return new HttpResult(false, HttpStatusCode.ServiceUnavailable, string.Empty, lastException?.Message ?? "Request failed");
    }

    private static TimeSpan ComputeBackoff(int attempt)
    {
        var milliseconds = Math.Min(250 * (1 << (attempt - 1)), 4_000);
        return TimeSpan.FromMilliseconds(milliseconds);
    }

    private static bool IsTransientStatusCode(HttpStatusCode statusCode)
        => statusCode is HttpStatusCode.RequestTimeout
            or HttpStatusCode.TooManyRequests
            or HttpStatusCode.BadGateway
            or HttpStatusCode.ServiceUnavailable
            or HttpStatusCode.GatewayTimeout;

    public void Dispose() => _httpClient.Dispose();
}
