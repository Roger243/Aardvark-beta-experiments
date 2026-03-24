using System.Text.RegularExpressions;

namespace KnowledgeBaseModule.Utils;

public static class StringSanitizer
{
    private static readonly Regex MultiWhitespace = new("\\s+", RegexOptions.Compiled);

    public static string Clean(string value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return string.Empty;
        }

        return MultiWhitespace.Replace(value, " ").Trim();
    }
}
