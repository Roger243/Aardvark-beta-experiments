using System;

namespace UserUiNotificationModule.Exceptions;

public sealed class UiModuleException : Exception
{
    public UiModuleException(string message)
        : base(message)
    {
    }

    public UiModuleException(string message, Exception innerException)
        : base(message, innerException)
    {
    }
}
