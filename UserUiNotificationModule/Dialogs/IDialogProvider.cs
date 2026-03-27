using System.Threading;
using System.Threading.Tasks;

namespace UserUiNotificationModule.Dialogs;

public interface IDialogProvider
{
    Task<bool> PromptUserYesNoAsync(string question, CancellationToken cancellationToken = default);
}
