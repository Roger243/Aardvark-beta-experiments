using System;
using System.Threading;
using System.Threading.Tasks;
using NetworkConnectivityModule.Models;

namespace NetworkConnectivityModule.Http;

public interface IApiClient
{
    Task<HttpResult> GetAsync(Uri requestUri, CancellationToken cancellationToken = default);
    Task<HttpResult> PostJsonAsync(Uri requestUri, string jsonPayload, CancellationToken cancellationToken = default);
}
