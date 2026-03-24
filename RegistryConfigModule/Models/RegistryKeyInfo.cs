using System.Collections.Generic;

namespace RegistryConfigModule.Models;

public sealed record RegistryKeyInfo(
    string Path,
    IReadOnlyDictionary<string, object?> Values,
    IReadOnlyList<RegistryKeyInfo> SubKeys);
