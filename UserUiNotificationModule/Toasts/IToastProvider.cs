using System.Threading;
using System.Threading.Tasks;

namespace UserUiNotificationModule.Toasts;

public interface IToastProvider
{
    Task ShowToastAsync(string title, string message, CancellationToken cancellationToken = default);
}
