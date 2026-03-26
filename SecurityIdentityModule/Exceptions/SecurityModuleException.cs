using System;

namespace SecurityIdentityModule.Exceptions;

public sealed class SecurityModuleException : Exception
{
    public SecurityModuleException(string message) : base(message) { }
    public SecurityModuleException(string message, Exception innerException) : base(message, innerException) { }
}
