using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace KnowledgeBaseModule.Indexing;

public sealed class DocumentParser
{
    public async Task<IReadOnlyList<(string Path, string Content)>> ParseAsync(string path, CancellationToken cancellationToken = default)
    {
        var files = new List<string>();
        if (File.Exists(path))
        {
            files.Add(path);
        }
        else if (Directory.Exists(path))
        {
            files.AddRange(Directory.GetFiles(path, "*.md", SearchOption.AllDirectories));
            files.AddRange(Directory.GetFiles(path, "*.txt", SearchOption.AllDirectories));
        }

        var items = new List<(string Path, string Content)>();
        foreach (var file in files)
        {
            cancellationToken.ThrowIfCancellationRequested();
            var content = await File.ReadAllTextAsync(file, cancellationToken).ConfigureAwait(false);
            items.Add((file, content));
        }

        return items;
    }
}
