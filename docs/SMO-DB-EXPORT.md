# Generating Database Schema Scripts Without SSMS

> Companion doc for `../scripts/smo-db-export3.ps1`

SSMS "Generate Scripts" is just a UI wrapper around **SMO (SQL Server Management Objects)**. The script drives the same engine from PowerShell, producing output equivalent to the `sql/LosaGame_UNICODE.sql` SSMS export — including the `/****** Object: ... ******/` headers, `GO` batch terminators, and `SET ANSI_NULLS ON` blocks — verified statement-by-statement against the SSMS output.

## Requirements

| Environment | SMO source | Notes |
|-------------|-----------|-------|
| **PowerShell 7+** | `SqlServer` module (SMO 22.x) from PowerShell Gallery | `Install-Module SqlServer -Scope CurrentUser` |

The script is PowerShell 7 only — it uses the `SqlServer` module and does not fall back to the SSMS 18 .NET Framework assemblies.

## Usage

```powershell
# Full database schema, Linux target (e.g. SQL Server in Docker)
.\scripts\smo-db-export3.ps1 `
  -Server "172.30.189.8,1500" `
  -Database "LosaGame" `
  -User "sa" `
  -Password "***" `
  -Output "sql\linux\LosaGame_UNICODE.sql" `
  -Encoding UNICODE `
  -Target Linux
```

A full `LosaGame` export takes ~2–3 minutes and produces ~3.2 MB (schema only, no data).

### Parameters

| Parameter   | Required | Description |
|-------------|----------|-------------|
| `Server`    | Yes      | Host and port, e.g. `172.30.189.8,1500` |
| `Database`  | Yes      | Database to script, e.g. `LosaGame` |
| `User`      | Yes      | SQL login |
| `Password`  | Yes      | SQL password |
| `Output`    | Yes      | Output file path (created if missing) |
| `Encoding`  | No       | `ANSI`, `ASCII`, `UTF8`, `UTF8BOM`, `UNICODE`, `UTF32`. Defaults to `UNICODE` |
| `Target`    | No       | `Windows` or `Linux`. Defaults to `Windows` |

### Encoding notes

- `ANSI` — **CP949** (Korean Windows ANSI code page), so Korean comments inside stored procedures survive intact. Matches the legacy `LosaGame_ANSI.sql` in `sql/`.
- `UNICODE` — **UTF-16 LE + BOM**. Closest match to SSMS Unicode output.
- `UTF8` — UTF-8 without BOM; `UTF8BOM` — UTF-8 with BOM.
- `ASCII` — plain 7-bit ASCII. **Korean text (CP949) will be lost** — use `ANSI`, `UNICODE`, or `UTF8BOM` for this database.
- `UTF32` — UTF-32 LE.

For reference: the CP949 file is ~1.6 MB; the equivalent UTF-16 file is ~3.2 MB.

### Target platform (`-Target`)

SMO emits `FILENAME = N'...'` paths pointing at the **source server's** data directory. The script rewrites them to the target platform's default data directory, keeping the original file names (`LosaGame.mdf` / `LosaGame_log.ldf`):

| Target    | Data path |
|-----------|-----------|
| `Windows` | `C:\Program Files\Microsoft SQL Server\MSSQL\DATA\` |
| `Linux`   | `/var/opt/mssql/data/` |

## What gets scripted

In a valid creation order:

1. `CREATE DATABASE` + all `ALTER DATABASE ... SET` settings (34 for `LosaGame`), fulltext check, `COMPATIBILITY_LEVEL`, and explicit `COLLATE`
2. Database users (`CREATE USER` + `GRANT CONNECT`) — emitted by `Database.Script()` itself; the users loop skips users already scripted
3. Schemas
4. Tables — with clustered/nonclustered/XML/spatial indexes, PK/FK/unique/check/default constraints, compression, triggers, extended properties
5. Views
6. Stored procedures
7. User-defined functions
8. Sequences, synonyms
9. Database roles (fixed roles excluded) and permissions (`GRANT`/`DENY`)
10. Database triggers

`USE [LosaGame]` is written once after the database section; objects follow.

## Known SMO quirks handled

These are the pitfalls found while building the script — all handled inside it:

1. **`WithDependencies` is unsupported for `Database`, `Schema`, `DatabaseRole`, and `User` object types** — SMO dependency discovery rejects them outright ("Database is not supported in dependency discovery"). The option is kept off globally; the loop order above is the dependency ordering.
2. **`WithDependencies = $true` duplicates objects massively.** Every procedure referencing a table re-emits that table's `CREATE TABLE` (`userMemberDB` appeared 210x; output ballooned from ~3.2 MB to 11 MB, runtime from ~3 min to ~10 min).
3. **`IncludeDatabaseContext = $true` emits `USE [LosaGame]` + `GO` before every single object** (1200+ duplicates). It is disabled; the script writes `USE` once.
4. **`Database.CompatibilityVersion` returns `$null` in SMO 22.x** (deprecated). Use `Database.CompatibilityLevel` — the enum's numeric value equals the level (`Version160` → 160).
5. **`ScriptingOptions.SetTargetServerVersion()` does not exist in SMO 22.x.** Set the `TargetServerVersion` property instead, mapping the server's major version to `[Microsoft.SqlServer.Management.Smo.SqlServerVersion]`.
6. **With `IncludeHeaders = $true`, the `/****** Object ... ******/` header is prepended inside the statement string itself** — regexes matching statements must not be anchored with `^`.
7. **`Database.Script()` also emits users and their permissions** (e.g. `GRANT CONNECT TO [lstggame] AS [dbo]`) — the users loop must skip already-scripted users or `CREATE USER` appears twice and fails on re-run.

## Differences vs the SSMS export

Compared to the hand-made SSMS export in `sql/LosaGame_UNICODE.sql`:

| Difference | Why |
|------------|-----|
| Includes newer objects (e.g. `userOAuthDB`) | The SSMS export is dated; SMO scripts the live database at run time |
| No `EXEC sys.sp_db_vardecimal_storage_format` | Legacy check, obsolete since SQL Server 2008 (vardecimal is always on); SMO 22.x dropped it |
| Extra `GRANT CONNECT` / `GRANT VIEW ANY ...` lines | `Permissions = $true` scripts real database permissions; SSMS "Generate Scripts" defaults omit them |
| Explicit `COLLATE Korean_Wansung_CI_AS` in `CREATE DATABASE` | Safer — the collation does not depend on the target server's default |
| Slightly more lines / `GO` batches | Formatting only — the writer adds `GO` + blank line after each statement |
