# NOVERA OSS — Lost Saga Server

**Copyright NOVERA OSS.** A revived and modernized server suite for the classic
Korean action-brawler *Lost Saga* — migrated from its original VS2008 / C++98
codebase to **Visual Studio 2022, C++17**, validated end-to-end with the
original game client.

> ⚠️ **Disclaimer** — This project is released for **educational and
> game-preservation purposes only**. It is not affiliated with, endorsed by,
> or connected to the original developers or publishers of Lost Saga. No
> game client, assets, or copyrighted media are included in this repository.
> You must provide your own game client and data files.

---

## 📖 About

This repository contains the complete server-side software stack for running a
Lost Saga game service: 7 server executables, 4 shared libraries, 4 auxiliary
libraries, database schemas, and deployment tooling.

In 2026 the codebase was migrated from C++98 / Visual Studio 2008 projects to:

| | |
|---|---|
| Toolset | Visual Studio 2022 (`v143`) |
| Language | C++17 |
| Platform | Win32 (x86, 32-bit) — required by the legacy binary protocol |
| Network | IPv4 only |

The full suite was rebuilt (Debug + Release), then validated in-game: client
login, lobby, rooms, gameplay, and database persistence all work against the
modernized binaries with **zero changes to the game client**.

---

## 🏗️ Architecture

All servers share the same core pattern: an IOCP worker pool feeds a
lock-free MPSC queue drained by a **single logic thread** per server, which
owns all game state. Database access is fully asynchronous via a dedicated
DB-proxy process.

```
 game client ──TCP──> ls_loginsvr (optional, load balancer for AutoUpgrade)
 game client ──TCP──> ls_gamesvr   (gameplay, lobby, rooms, shop, guild UI)
 game client ──UDP──> ls_gamesvr / ls_relaysvr (in-room realtime relay)
 game client ──TCP──> ls_filewritesvr (custom skin uploads)

 ls_gamesvr ──TCP──> ls_mainsvr   (guilds, trade market, tournaments,
                                    matchmaking, admin-tool routing)
 ls_gamesvr ──TCP──> ls_billingsvr (cash / purchase relay)
 ls_mainsvr ──TCP──> ls_dbagent ×2 (game DB + log DB) ──ADO──> SQL Server
 ls_gamesvr ──TCP──> ls_dbagent
 ls_billingsvr ──HTTP──> billing partners (optional, regional)
 ops tool   ──TCP──> ls_mainsvr / ls_billingsvr (manager ports)
```

### Components

| Project | Type | Role |
|---|---|---|
| `iocpSocketDLL` | DLL | Shared network core: IOCP, MPSC queue, packet class, pools |
| `Log` | static lib | Async threaded logging |
| `ioINILoader` | static lib | INI configuration reader/writer |
| `FrameTimerDLL` | DLL | QPC-based timer (`TIMEGETTIME` clock) |
| `tinyxml` | static lib | XML parsing (billing web responses) |
| `LS_HTTP` | static lib | HTTP client wrapper (WinINet) |
| `LS_NXSoap` | static lib | gSOAP runtime for Nexon SOAP billing |
| `LS_RestAPI` | static lib | REST client wrapper (libcurl) |
| `ls_gamesvr` | exe | **Game server** — rooms, modes, characters, shop |
| `ls_mainsvr` | exe | **Coordinator** — guilds, trade, ladder, tournaments |
| `ls_billingsvr` | exe | **Billing relay** — cash queries, regional partners |
| `ls_dbagent` | exe | **DB proxy** — ADO/Stored procedures, run ×2 (game + log) |
| `ls_filewritesvr` | exe | **Upload server** — player skin images |
| `ls_loginsvr` | exe | Load balancer for the AutoUpgrade launcher *(not yet modernized)* |
| `ls_relaysvr` | exe | Dedicated UDP relay *(optional; not yet modernized)* |

### Modernization status

| Server | Status |
|---|---|
| iocpSocketDLL, Log, ioINILoader, FrameTimerDLL, tinyxml, LS_HTTP, LS_NXSoap, LS_RestAPI | ✅ migrated, Debug + Release |
| ls_gamesvr, ls_mainsvr, ls_billingsvr, ls_dbagent, ls_filewritesvr | ✅ migrated + in-game validated |
| ls_relaysvr, ls_loginsvr | ⏳ original VS2008 sources included, migration pending (optional for gameplay) |

---

## 🔧 Build Requirements

1. **Visual Studio 2022** with the *Desktop development with C++* workload
   **and the "C++ ATL" component** (used by `atltime.h` / LS_HTTP).
2. **Boost 1.84** (prebuilt Windows binaries) installed at `C:\Boost\boost_1_84_0`
   (headers used by `BoostPooler` / `boost::unordered` / date_time; the
   `lib32-msvc-14.3` folder is only needed if you modernize relaysvr/loginsvr).
3. **vcpkg** with a static 32-bit libcurl (used by `LS_RestAPI`):
   ```powershell
   git clone https://github.com/microsoft/vcpkg D:\Tools\vcpkg
   cd D:\Tools\vcpkg; .\bootstrap-vcpkg.bat
   .\vcpkg.exe install curl:x86-windows-static
   # copy libcurl.lib / libcurl-d.lib + zlib (zs.lib, zsd.lib) into .\lib\
   # and curl headers into .\include\libcurl\
   ```
4. Everything else (OpenSSL import libs, CrashFind, NMCrypt, mini-dump
   tooling) is already included in-tree.

All projects target **Win32** with `PlatformToolset=v143` and
`LanguageStandard=c++17`. No 64-bit or non-Windows builds are supported
(the wire protocol assumes 32-bit x86 layout).

---

## 🛠️ Building

Project files live in `win\projects\*.vcxproj` (solutions in `win\`).
Build in this order (each step: Debug **and** Release):

```
1.  iocpSocketDLL          5.  tinyxml          9.  ls_dbagent
2.  Log                    6.  LS_HTTP          10. ls_billingsvr
3.  ioINILoader            7.  LS_NXSoap        11. ls_mainsvr
4.  FrameTimerDLL          8.  LS_RestAPI       12. ls_gamesvr
                                              (13. ls_filewritesvr)
```

Notes:

- `ls_gamesvr` links with `/SAFESEH:NO` (a bundled legacy static library is
  not SAFESEH-compatible).
- Windows SDK libs required by `ls_billingsvr` beyond the defaults:
  `crypt32, secur32, normaliz, wldap32, iphlpapi, bcrypt`.

---

## 🗄️ Database Setup

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

The two `ls_dbagent` instances are configured through:

| File | Purpose |
|---|---|
| `ls_dbagent_game.ini` | Game DB agent — SQL connection + listen port |
| `ls_dbagent_log.ini` | Log DB agent — logger mode (`LogServerPort` set) |
| `ls_query.ini` | Query registry — maps query IDs to stored procedures |

---

## 🚀 Deployment & Running

Binaries and configs are deployed under `build\Server\`:

```
build\Server\
├── global_define.ini          # inter-server topology (IPs/ports)
├── start_all.bat             # launches everything in dependency order
├── start_dbagent_game.bat    # individual starters (console mode, -c INI)
├── start_dbagent_log.bat
├── start_billingsvr.bat
├── start_mainsvr.bat
├── start_gamesvr.bat
├── start_filewritesvr.bat
└── ls_<server>\              # exe (Debug+Release) + required DLLs + INIs
```

### Runtime DLLs (must sit next to each exe)

| Server | Required DLLs |
|---|---|
| all | `iocpSocketDLL.dll`, `iocpSocketDDLL.dll`, `dbghelp.dll` |
| all except dbagent | `CrashFind.dll`, `frametimerdll.dll`, `frametimerddll.dll` |
| ls_billingsvr | + `libeay32.dll`, `ssleay32.dll` (bundled in `src\ls_billingsvr\Openssl`) |

(`D`-suffixed DLLs are the Debug variants; both are needed when both exe
types are deployed in the same folder. `gdiplus`, `wininet`, `winhttp` come
with Windows.)

### Start order

`start_all.bat` handles this: **dbagent (game) → dbagent (log) → billingsvr
→ mainsvr → gamesvr → filewritesvr**. Gamesvr requires mainsvr, billingsvr
and the dbagents to be reachable before players can log in.

### Operations

`ls_dbagent` prints per-thread queue statistics (`QPOP/QPUSH/QR`) to
`ReportLOG` every 10 minutes — a steadily climbing `QR` (queue remainder)
means the database is falling behind; check SQL latency before adding game
server instances.

### Scaling

The game server is a **single-logic-thread** design: one `ls_gamesvr` zone
uses ~1 CPU core. To scale, run multiple gamesvr instances (copy
`ls_gamesvr_1.ini` → `_2.ini`, `_3.ini`, ... with distinct ports) and list
them in `global_define.ini` — the architecture is horizontally scaled by
design.

---

## 📄 License

Copyright NOVERA OSS. Licensed under the **GNU General Public License v3** —
see [LICENSE](LICENSE) for details.

This project is not affiliated with the original developers or publishers
of Lost Saga and is intended for educational and preservation purposes.
