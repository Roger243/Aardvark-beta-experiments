using System;

namespace KnowledgeBaseModule.Exceptions;

public sealed class KnowledgeModuleException : Exception
{
    public KnowledgeModuleException(string message) : base(message) { }
    public KnowledgeModuleException(string message, Exception innerException) : base(message, innerException) { }
}
