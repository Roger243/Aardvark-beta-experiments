using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using ContextMemoryManager.History;

namespace ContextMemoryManager;

public sealed class SessionContext
{
    private readonly int _maxEntries;
    private readonly Queue<ExecutionLogEntry> _entries;
    private readonly ReaderWriterLockSlim _rwLock = new(LockRecursionPolicy.NoRecursion);

    public SessionContext(int maxEntries = 20)
    {
        if (maxEntries is < 10 or > 200)
        {
            throw new ArgumentOutOfRangeException(nameof(maxEntries), "Session memory size should be in a safe range (10-200).");
        }

        _maxEntries = maxEntries;
        _entries = new Queue<ExecutionLogEntry>(maxEntries);
    }

    public int MaxEntries => _maxEntries;

    public int Count
    {
        get
        {
            _rwLock.EnterReadLock();
            try
            {
                return _entries.Count;
            }
            finally
            {
                _rwLock.ExitReadLock();
            }
        }
    }

    public void Add(ExecutionLogEntry entry)
    {
        _rwLock.EnterWriteLock();
        try
        {
            while (_entries.Count >= _maxEntries)
            {
                _entries.Dequeue();
            }

            _entries.Enqueue(entry);
        }
        finally
        {
            _rwLock.ExitWriteLock();
        }
    }

    public IReadOnlyList<ExecutionLogEntry> Snapshot()
    {
        _rwLock.EnterReadLock();
        try
        {
            return _entries.ToList();
        }
        finally
        {
            _rwLock.ExitReadLock();
        }
    }

    public IReadOnlyList<ExecutionLogEntry> Recent(int count)
    {
        if (count <= 0) return Array.Empty<ExecutionLogEntry>();

        _rwLock.EnterReadLock();
        try
        {
            return _entries.Reverse().Take(count).Reverse().ToList();
        }
        finally
        {
            _rwLock.ExitReadLock();
        }
    }
}
