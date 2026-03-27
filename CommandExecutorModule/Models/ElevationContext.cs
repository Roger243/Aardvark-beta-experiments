using System;

namespace CommandExecutorModule.Models;

public enum ElevationLevel
{
    Standard = 0,
    Elevated = 1,
    System = 2
}

public sealed record ElevationContext
{
    public ElevationLevel RequiredLevel { get; init; } = ElevationLevel.Standard;

    /// <summary>
    /// If true and elevation is needed, the command is re-launched in a new elevated process using the UAC prompt.
    /// </summary>
    public bool AllowUacPrompt { get; init; } = true;

    /// <summary>
    /// If true, throw when elevation cannot be achieved (instead of executing best-effort).
    /// </summary>
    public bool StrictRequirement { get; init; } = true;

    /// <summary>
    /// Optional timeout for spawned elevated process execution.
    /// </summary>
    public TimeSpan? Timeout { get; init; }

    public static ElevationContext Standard() => new() { RequiredLevel = ElevationLevel.Standard };
    public static ElevationContext Elevated() => new() { RequiredLevel = ElevationLevel.Elevated };
    public static ElevationContext System() => new() { RequiredLevel = ElevationLevel.System };
}
