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

`ls_dbagent` also has a small utility mode to encode credentials:
`ls_dbagent.exe -x <password>` prints the encoded string for use in the
dbagent INI.
