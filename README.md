# NOVERA Legacy

**Copyright NOVERA OSS.** The revived source tree of the classic Korean
action-brawler *Lost Saga* — game client, server suite, launcher, and
tooling — now building with Visual Studio 2022 (C++17).

> ⚠️ **Disclaimer** — This project is released for **educational and
> game-preservation purposes only**. It is not affiliated with, endorsed by,
> or connected to the original developers or publishers of Lost Saga. No
> game client binaries, assets, or copyrighted media are included in this
> repository. You must provide your own game client and resource files.

---

## Project direction

NOVERA Legacy is the open, self-hostable continuation of Lost Saga. There is
no official server to join and none is planned — the tree ships complete
source, databases, and runtime configuration so anyone can run the game on
their own machine. The project preserves the game and improves it:
incremental, real changes, not a replica of any specific Korean season.
Contributions are welcome — see [CONTRIBUTING.md](CONTRIBUTING.md).

## What's inside

Nearly the entire game lives in this tree: client, engine, servers,
launcher, patcher, monitoring, and the tools used to build and operate it.

### Game client

| Path | Contents |
|---|---|
| `src/LSClient/` | The complete game client — 3,600+ files: gameplay modes, characters, skills, items, UI, networking |
| `src/io3DEngine/` | The client's 3D rendering and scene engine |
| `src/ioPac/` | Resource package (`.iop`) reader/writer |
| `src/ioFreeType/`, `src/OggVorbis/` | Text rendering and audio decoding |
| `src/FlashPlayerToDirectX/` | Flash-in-DirectX layer (UI movies) |
| `src/ioWinHttp/`, `src/ErrorDlg/`, `src/TownPortal/` | HTTP, error dialogs, and client support components |

### Server suite

| Path | Contents |
|---|---|
| `src/ls_gamesvr/` | Game server — login, lobby, rooms, modes, shop, guild UI |
| `src/ls_mainsvr/` | Coordinator — guilds, trade market, tournaments, matchmaking |
| `src/ls_billingsvr/` | Billing relay — cash queries, regional payment partners |
| `src/ls_dbagent/` | Asynchronous DB proxy — ADO + stored procedures (run one per database: game + log) |
| `src/ls_filewritesvr/` | Player skin upload server |
| `src/ls_loginsvr/`, `src/ls_relaysvr/` | Optional — patcher load balancer and dedicated UDP relay |
| `src/iocpSocketDLL/`, `src/Log/`, `src/ioINILoader/`, `src/FrameTimerDLL/` | Shared plumbing — IOCP network core, logging, INI config, timers |

### Launcher, patcher & tooling

| Path | Contents |
|---|---|
| `src/LSPacketSniffer/` | Npcap-based packet analyzer for the Lost Saga wire protocol |
| `src/LSAutoUpgrade/`, `src/PatchManager/`, `src/LSWebBroker/` | Launcher/upgrader, patch manager, web broker |
| `src/LSMonitor/`, `src/LSController/`, `src/LSLog/`, `src/LSLogClient/`, `src/PerfMon/` | Monitoring, control, and log tooling |
| `src/VCSGen/`, `src/gperftools-2.1/`, `src/ziparchive320/` | Version-stamp generator, profiler, zip archive support |

### The rest of the tree

| Path | Contents |
|---|---|
| `extra/` | Developer utilities — opcode-to-string parser, encoder, CRC/INI checkers, billing lists, static libs |
| `Tools/` | Prebuilt developer tools (LSPacketSniffer, LSController, VCSGen) |
| `sql/` | SQL Server schemas — `LosaGame`, `LosaGame_log`, `LosaLogData` (ANSI + UNICODE variants) |
| `data/` | Sanitized runtime configuration — server and client templates ([guide](docs/CONFIG.md)) |
| `win/` | Visual Studio 2022 solutions and project files |
| `lib/` | Bundled legacy binaries — CrashFind, FlashDX, OpenSSL, frame timer, and friends |
| `docs/` | [Building](docs/BUILDING.md), [database setup](docs/DATABASE.md), [configuration](docs/CONFIG.md), [release process](docs/release.md), and [SMO export](docs/SMO-DB-EXPORT.md) guides |

## Running it yourself

There is no official server and no prebuilt binary distribution — the tree
is meant to be built and run on your own machine:

1. **Build** the client and server suite — see [docs/BUILDING.md](docs/BUILDING.md).
2. **Restore the databases** — see [docs/DATABASE.md](docs/DATABASE.md).
3. **Deploy the runtime configuration** from `data/` — see
   [docs/CONFIG.md](docs/CONFIG.md).
4. **Start the servers** in dependency order: dbagent → billingsvr →
   mainsvr → gamesvr → filewritesvr.

Releases on the mirrors carry automatically generated notes; the source at
each tag is the complete package.

## License

Copyright NOVERA OSS. Licensed under the **GNU General Public License v3** —
see [LICENSE](LICENSE) for details.

This project is not affiliated with the original developers or publishers
of Lost Saga and is intended for educational and preservation purposes.
