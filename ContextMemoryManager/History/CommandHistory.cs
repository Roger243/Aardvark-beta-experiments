using System.Collections.Generic;

namespace ContextMemoryManager.History;

public sealed record CommandHistory(IReadOnlyList<ExecutionLogEntry> Entries);
