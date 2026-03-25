using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using UserUiNotificationModule.Exceptions;

namespace UserUiNotificationModule.Dialogs;

public sealed class NativeDialogProvider : IDialogProvider
{
    private readonly SemaphoreSlim _dialogSemaphore = new(1, 1);

    public async Task<bool> PromptUserYesNoAsync(string question, CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        await _dialogSemaphore.WaitAsync(cancellationToken).ConfigureAwait(false);

        try
        {
            var tcs = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);

            var thread = new Thread(() =>
            {
                try
                {
                    var result = MessageBox.Show(
                        question,
                        "WinAgent",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Question,
                        MessageBoxDefaultButton.Button2,
                        MessageBoxOptions.DefaultDesktopOnly);

                    tcs.TrySetResult(result == DialogResult.Yes);
                }
                catch (Exception ex)
                {
                    tcs.TrySetException(new UiModuleException("Failed to display Yes/No prompt.", ex));
                }
            });

            thread.SetApartmentState(ApartmentState.STA);
            thread.IsBackground = true;
            thread.Start();

            using var registration = cancellationToken.Register(() => tcs.TrySetCanceled(cancellationToken));
            return await tcs.Task.ConfigureAwait(false);
        }
        finally
        {
            _dialogSemaphore.Release();
        }
    }
}
