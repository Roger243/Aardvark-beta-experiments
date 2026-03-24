using System;
using System.Linq;

namespace SystemMonitorModule.Wmi;

public static class WmiQueryBuilder
{
    public static string SelectFrom(string className, params string[] fields)
    {
        if (string.IsNullOrWhiteSpace(className))
        {
            throw new ArgumentException("WMI class name is required.", nameof(className));
        }

        if (fields is null || fields.Length == 0)
        {
            throw new ArgumentException("At least one WMI field is required.", nameof(fields));
        }

        var sanitizedFields = fields.Select(SanitizeIdentifier);
        var sanitizedClass = SanitizeIdentifier(className);
        return $"SELECT {string.Join(", ", sanitizedFields)} FROM {sanitizedClass}";
    }

    private static string SanitizeIdentifier(string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new ArgumentException("Identifier cannot be empty.", nameof(value));
        }

        if (value.Any(ch => !(char.IsLetterOrDigit(ch) || ch == '_' || ch == '.')))
        {
            throw new ArgumentException($"Invalid WMI identifier '{value}'.", nameof(value));
        }

        return value;
    }
}
