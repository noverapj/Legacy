# NOVERA Legacy

**Copyright NOVERA OSS.** A revived and modernized server suite for the classic
Korean action-brawler *Lost Saga* — migrated from its original VS2008 / C++98
codebase to **Visual Studio 2022, C++17**.

> ⚠️ **Disclaimer** — This project is released for **educational and
> game-preservation purposes only**. It is not affiliated with, endorsed by,
> or connected to the original developers or publishers of Lost Saga. No
> game client, assets, or copyrighted media are included in this repository.
> You must provide your own game client and data files.

---

## About

This repository contains the complete server-side software stack for running a
Lost Saga game service: 7 server executables, shared and auxiliary libraries,
database schemas, and tooling.

| | |
|---|---|
| Toolset | Visual Studio 2022 (`v143`) |
| Language | C++17 |
| Platform | Win32 (x86, 32-bit) — required by the legacy binary protocol |
| Network | IPv4 only |

The core gameplay path (client login, lobby, rooms, gameplay, database
persistence) was validated end-to-end against the modernized binaries with
**zero changes to the game client**.

## Architecture

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
| `ls_loginsvr` | exe | Load balancer for the AutoUpgrade launcher *(optional)* |
| `ls_relaysvr` | exe | Dedicated UDP relay *(optional)* |

### Modernization status

All libraries and all 7 server executables are migrated to v143 / C++17.

| Scope | Status |
|---|---|
| Libraries (iocpSocketDLL, Log, ioINILoader, FrameTimerDLL, tinyxml, LS_HTTP, LS_NXSoap, LS_RestAPI) | ✅ migrated, Debug + Release |
| ls_gamesvr, ls_mainsvr, ls_billingsvr, ls_dbagent, ls_filewritesvr | ✅ migrated + in-game validated |
| ls_loginsvr, ls_relaysvr | ✅ migrated, Debug + Release build-verified |

## Quick Start

1. **Build** — toolchain requirements and build order: [docs/BUILDING.md](docs/BUILDING.md)
2. **Database** — restore schemas and configure the DB agents: [docs/DATABASE.md](docs/DATABASE.md)
3. **Run** — deploy and start the servers (see below)

## Running

Each server runs in console mode with `-c <config.ini>` (the INI path is
resolved relative to the folder you launch from):

```
ls_dbagent.exe -c ls_dbagent_game.ini      (game DB agent)
ls_dbagent.exe -c ls_dbagent_log.ini       (log DB agent)
ls_<server>.exe -c <config.ini>           (all other servers)
```

Alternatively, each exe can be registered as a Windows Service
(`exe -i <svcname> "<display name>"`), then started with
`sc start <svcname> <config.ini> <logfile>` (the INI argument is
**mandatory** in service mode).

**Start order:** dbagent (game) → dbagent (log) → billingsvr → mainsvr →
gamesvr → filewritesvr. `ls_loginsvr` and `ls_relaysvr` are optional and
reconnect automatically, so they can be started any time after the core set
is up.

**Runtime DLLs** (must sit next to each exe):

| Server | Required DLLs |
|---|---|
| all | `iocpSocketDLL.dll`, `iocpSocketDDLL.dll`, `dbghelp.dll` |
| 5 core servers (not dbagent, not loginsvr/relaysvr) | + `CrashFind.dll` |
| all except dbagent | + `frametimerdll.dll`, `frametimerddll.dll` |
| ls_billingsvr | + `libeay32.dll`, `ssleay32.dll` (bundled in `src\ls_billingsvr\Openssl`) |

(`D`-suffixed DLLs are the Debug variants; both are needed when both exe
types are deployed in the same folder. `gdiplus`, `wininet`, `winhttp` come
with Windows.)

**Bind address** (`ls_loginsvr` / `ls_relaysvr` only): optional `IP` key in
the `[Default]` section of the config INI (default `0.0.0.0` = all
interfaces); an unavailable address fails loudly at startup.

**Scaling:** the game server is a single-logic-thread design — one
`ls_gamesvr` zone uses ~1 CPU core. Scale horizontally by running multiple
gamesvr instances (copy `ls_gamesvr_1.ini` → `_2.ini`, `_3.ini`, ... with
distinct ports) and listing them in `global_define.ini`.

**Monitoring:** `ls_dbagent` prints per-thread queue statistics
(`QPOP/QPUSH/QR`) to `ReportLOG` every 10 minutes — a steadily climbing
`QR` means the database is falling behind.

> `build\` is not tracked by git — it is generated locally, and no starter
> scripts are included in the repository. Create your own deployment folder
> and copy the binaries, DLLs, and config INIs there.

## License

Copyright NOVERA OSS. Licensed under the **GNU General Public License v3** —
see [LICENSE](LICENSE) for details.

This project is not affiliated with the original developers or publishers
of Lost Saga and is intended for educational and preservation purposes.
