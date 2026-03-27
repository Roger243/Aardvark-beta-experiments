namespace ContextMemoryManager.Utils;

public sealed class TokenCounter
{
    // Lightweight approximation for planning context windows.
    public int EstimateTokens(string text)
    {
        if (string.IsNullOrEmpty(text)) return 0;
        return (text.Length + 3) / 4;
    }
}
