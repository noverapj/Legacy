# Database Setup

Requires **SQL Server** (runs fine on SQL Server for Linux).

Each database ships in two encodings — pick one per database:

| Script (UNICODE — recommended) | Script (ANSI) | Database name |
|---|---|---|
| `sql\LosaLogData_UNICODE.sql` | `sql\LosaLogData_ANSI.sql` | `LosaLogData` |
| `sql\LosaGame_Log_UNICODE.sql` | `sql\LosaGame_Log_ANSI.sql` | `LosaGame_log` |
| `sql\LosaGame_UNICODE.sql` | `sql\LosaGame_ANSI.sql` | `LosaGame` |

- **UNICODE** scripts are encoding-safe — use them on any locale.
- **ANSI** scripts are Windows-949 (cp949) encoded — only use them when your
  tooling explicitly expects ANSI/Korean codepage input.
- Restore the **log databases first**, then the game database (the game DB
  contains cross-database procedures referencing the log DBs).
- The `CREATE DATABASE` headers contain the original production Windows
  paths — either edit them, or (recommended) strip the header block and
  pre-create each database yourself. On Linux, data files conventionally go
  to `/var/opt/mssql/data/`.

## DB agent configuration

The two `ls_dbagent` instances are configured through:

| File | Purpose |
|---|---|
| `ls_dbagent_game.ini` | Game DB agent — SQL connection + listen port |
| `ls_dbagent_log.ini` | Log DB agent — logger mode (`LogServerPort` set) |
| `ls_query.ini` | Query registry — maps query IDs to stored procedures |

Template copies live in `data/server/ls_dbagent/` — see
[CONFIG.md](CONFIG.md).

### Password encoding

The `[SQL] PW=` value is not the plaintext password. It is produced by a
double XOR against a fixed 30-byte key, then hex encoded
(`cQueryManager::Encode` in `src/ls_dbagent/Database/cQueryManager.cpp`).
The agent decodes it at startup.

To generate it for your own SQL password, run the encoder mode:

```
ls_dbagent.exe -x <plaintext-password>
```

Paste the printed string into `PW=` of both `ls_dbagent_game.ini` and
`ls_dbagent_log.ini`.

The encoding is reversible obfuscation, not a hash — never commit real or
encoded credentials under `data/`. The published templates carry
`PW=CHANGE_ME`; the pre-commit hook blocks credential-shaped values.
