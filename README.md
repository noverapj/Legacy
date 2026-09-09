# NOVERA Legacy

**Copyright NOVERA OSS.** A revived server suite for the classic Korean
action-brawler *Lost Saga* — the complete production stack, modernized to
**Visual Studio 2022 / C++17**, and validated against the modern Korean game
client with **zero client-side modification**.

> ⚠️ **Disclaimer** — This project is released for **educational and
> game-preservation purposes only**. It is not affiliated with, endorsed by,
> or connected to the original developers or publishers of Lost Saga. No
> game client, assets, or copyrighted media are included in this repository.
> You must provide your own game client and data files.

---

## Highlights

- **Runs the modern KR client as-is** — no client patches, no protocol
  emulation. Login, lobby, rooms, and gameplay all speak the original wire
  protocol, end to end.
- **Complete stack** — 7 server executables plus shared libraries and full
  SQL Server schemas: game, coordination, billing, DB proxy, file upload,
  and optional login balancer / UDP relay.
- **Single-logic-thread IOCP architecture** — one zone per core; scale
  horizontally by running more game server instances.
- **Protocol tooling in-tree** — `src/LSPacketSniffer`, an Npcap-based
  packet analyzer for the Lost Saga wire format, ships with the source.

## Getting the binaries

Prebuilt binaries are published on the
[Releases](https://github.com/noverapj/Legacy/releases) page — download the
latest archive, extract it, and:

1. **Restore the databases** — see [docs/DATABASE.md](docs/DATABASE.md).
2. **Configure the server INIs** (SQL connection, ports).
3. **Start the servers** in dependency order: dbagent → billingsvr →
   mainsvr → gamesvr → filewritesvr.

To build from source instead, see [docs/BUILDING.md](docs/BUILDING.md).

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

Five core services — `ls_gamesvr` (gameplay, lobby, rooms, shop),
`ls_mainsvr` (guilds, trade, tournaments, matchmaking), `ls_billingsvr`
(cash relay), `ls_dbagent` (async DB proxy, run twice: game + log), and
`ls_filewritesvr` (skin uploads) — plus two optional helpers
(`ls_loginsvr` launcher load balancer, `ls_relaysvr` dedicated UDP relay),
all built on a shared `iocpSocketDLL` network core (IOCP, MPSC queue,
packet pools) with common utility libraries.

Everything is migrated to `v143` / C++17 (Debug + Release); the five core
servers are validated in-game.

## Documentation

| Doc | Contents |
|---|---|
| [docs/BUILDING.md](docs/BUILDING.md) | Toolchain requirements and build order |
| [docs/DATABASE.md](docs/DATABASE.md) | SQL Server restore, encodings, DB agent configuration |

## License

Copyright NOVERA OSS. Licensed under the **GNU General Public License v3** —
see [LICENSE](LICENSE) for details.

This project is not affiliated with the original developers or publishers
of Lost Saga and is intended for educational and preservation purposes.
