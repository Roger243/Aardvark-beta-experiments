using System;

namespace SecurityIdentityModule.Models;

public sealed record SecretEntry(
    string Key,
    byte[] Ciphertext,
    DateTimeOffset CreatedUtc,
    DateTimeOffset UpdatedUtc);
