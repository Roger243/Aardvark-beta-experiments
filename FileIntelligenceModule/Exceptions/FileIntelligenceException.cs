using System;

namespace FileIntelligenceModule.Exceptions;

public sealed class FileIntelligenceException : Exception
{
    public FileIntelligenceException(string message, Exception? innerException = null) : base(message, innerException) { }
}
