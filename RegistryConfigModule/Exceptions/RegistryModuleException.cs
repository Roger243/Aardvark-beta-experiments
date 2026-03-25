using System;

namespace RegistryConfigModule.Exceptions;

public sealed class RegistryModuleException : Exception
{
    public RegistryModuleException(string message)
        : base(message)
    {
    }

    public RegistryModuleException(string message, Exception innerException)
        : base(message, innerException)
    {
    }
}
