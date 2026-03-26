using System;

namespace SelfDiagnosticModule.Exceptions;

public sealed class SelfDiagnosticException : Exception
{
    public SelfDiagnosticException(string message) : base(message) { }
    public SelfDiagnosticException(string message, Exception innerException) : base(message, innerException) { }
}
