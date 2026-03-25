using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Dapper;
using KnowledgeBaseModule.Exceptions;
using KnowledgeBaseModule.Models;
using Microsoft.Data.Sqlite;

namespace KnowledgeBaseModule.Storage;

public sealed class SqliteKnowledgeStore : IKnowledgeStore
{
    private readonly string _connectionString;

    public SqliteKnowledgeStore(string sqlitePath)
    {
        _connectionString = $"Data Source={sqlitePath}";
    }

    public async Task InitializeAsync(CancellationToken cancellationToken = default)
    {
        const string sql = """
        CREATE TABLE IF NOT EXISTS Snippets (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Title TEXT NOT NULL,
            Content TEXT NOT NULL,
            Tags TEXT NOT NULL,
            Category TEXT NOT NULL,
            CreatedUtc TEXT NOT NULL,
            UpdatedUtc TEXT NOT NULL,
            ConfidenceScore REAL NOT NULL,
            IsArchived INTEGER NOT NULL DEFAULT 0
        );
        CREATE INDEX IF NOT EXISTS IX_Snippets_CreatedUtc ON Snippets(CreatedUtc);
        CREATE INDEX IF NOT EXISTS IX_Snippets_Category ON Snippets(Category);
        """;

        await using var conn = CreateConnection();
        await conn.OpenAsync(cancellationToken).ConfigureAwait(false);
        await conn.ExecuteAsync(new CommandDefinition(sql, cancellationToken: cancellationToken)).ConfigureAwait(false);
    }

    public async Task<long> InsertAsync(KnowledgeSnippet snippet, CancellationToken cancellationToken = default)
    {
        const string sql = """
        INSERT INTO Snippets (Title, Content, Tags, Category, CreatedUtc, UpdatedUtc, ConfidenceScore, IsArchived)
        VALUES (@Title, @Content, @Tags, @Category, @CreatedUtc, @UpdatedUtc, @ConfidenceScore, 0);
        SELECT last_insert_rowid();
        """;

        try
        {
            await using var conn = CreateConnection();
            await conn.OpenAsync(cancellationToken).ConfigureAwait(false);

            var id = await conn.ExecuteScalarAsync<long>(new CommandDefinition(
                sql,
                new
                {
                    snippet.Title,
                    snippet.Content,
                    Tags = string.Join(',', snippet.Tags),
                    snippet.Category,
                    CreatedUtc = snippet.CreatedUtc.UtcDateTime,
                    UpdatedUtc = snippet.UpdatedUtc.UtcDateTime,
                    snippet.ConfidenceScore
                },
                cancellationToken: cancellationToken)).ConfigureAwait(false);

            return id;
        }
        catch (Exception ex)
        {
            throw new KnowledgeModuleException("Failed to insert knowledge snippet.", ex);
        }
    }

    public async Task<IReadOnlyList<KnowledgeSnippet>> SearchAsync(SearchQuery query, CancellationToken cancellationToken = default)
    {
        var tags = query.Tags is { Count: > 0 }
            ? query.Tags.Select(t => t.Trim().ToLowerInvariant()).ToArray()
            : Array.Empty<string>();

        var sql = """
        SELECT Id, Title, Content, Tags, Category, CreatedUtc, UpdatedUtc, ConfidenceScore
        FROM Snippets
        WHERE IsArchived = 0
          AND (@Category IS NULL OR Category = @Category)
          AND (@CreatedAfterUtc IS NULL OR CreatedUtc >= @CreatedAfterUtc)
          AND (
               @QueryText = ''
               OR Title LIKE '%' || @QueryText || '%'
               OR Content LIKE '%' || @QueryText || '%'
               OR Tags LIKE '%' || @QueryText || '%'
          )
        ORDER BY UpdatedUtc DESC
        LIMIT @Limit;
        """;

        await using var conn = CreateConnection();
        await conn.OpenAsync(cancellationToken).ConfigureAwait(false);

        var rows = await conn.QueryAsync<SnippetRow>(new CommandDefinition(
            sql,
            new
            {
                QueryText = query.QueryText ?? string.Empty,
                query.Category,
                CreatedAfterUtc = query.CreatedAfterUtc?.UtcDateTime,
                Limit = Math.Clamp(query.Limit, 1, 200)
            },
            cancellationToken: cancellationToken)).ConfigureAwait(false);

        var snippets = rows.Select(r => new KnowledgeSnippet
        {
            Id = r.Id,
            Title = r.Title,
            Content = r.Content,
            Tags = SplitTags(r.Tags),
            Category = r.Category,
            CreatedUtc = new DateTimeOffset(DateTime.SpecifyKind(r.CreatedUtc, DateTimeKind.Utc)),
            UpdatedUtc = new DateTimeOffset(DateTime.SpecifyKind(r.UpdatedUtc, DateTimeKind.Utc)),
            ConfidenceScore = r.ConfidenceScore
        });

        if (tags.Length == 0)
        {
            return snippets.ToArray();
        }

        return snippets
            .Where(s => tags.All(t => s.Tags.Any(x => string.Equals(x, t, StringComparison.OrdinalIgnoreCase))))
            .ToArray();
    }

    public async Task<int> ArchiveOlderThanAsync(int days, CancellationToken cancellationToken = default)
    {
        const string sql = """
        UPDATE Snippets
        SET IsArchived = 1, UpdatedUtc = @UpdatedUtc
        WHERE IsArchived = 0 AND CreatedUtc < @CutoffUtc;
        """;

        await using var conn = CreateConnection();
        await conn.OpenAsync(cancellationToken).ConfigureAwait(false);

        var affected = await conn.ExecuteAsync(new CommandDefinition(
            sql,
            new
            {
                UpdatedUtc = DateTime.UtcNow,
                CutoffUtc = DateTime.UtcNow.AddDays(-Math.Abs(days))
            },
            cancellationToken: cancellationToken)).ConfigureAwait(false);

        return affected;
    }

    private SqliteConnection CreateConnection() => new(_connectionString);

    private static IReadOnlyList<string> SplitTags(string tags)
        => string.IsNullOrWhiteSpace(tags)
            ? Array.Empty<string>()
            : tags.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);

    private sealed record SnippetRow(
        long Id,
        string Title,
        string Content,
        string Tags,
        string Category,
        DateTime CreatedUtc,
        DateTime UpdatedUtc,
        double ConfidenceScore);
}
