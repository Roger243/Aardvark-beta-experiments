using System;

namespace ContextMemoryManager.Exceptions;

public sealed class MemoryException : Exception
{
    public MemoryException(string message, Exception? innerException = null) : base(message, innerException) { }
}
