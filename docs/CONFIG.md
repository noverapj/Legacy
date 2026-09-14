# Configuration and Data

Runtime configuration and game data are distributed in `data/`, versioned
together with the source code. A tag always carries source and configuration
that belong together — there is no separate config version to keep in sync.

```
data/
├── server/    per-service server configuration
└── client/    client-side configuration and XML data
```

## Server side (data/server/)

Deploy by copying the service folders next to their compiled binaries.
LSController.exe and config.ini sit at the root, with the service folders
around it — each server resolves its INI from its own working directory.

| Path | Purpose |
| --- | --- |
| `config.ini` | LSController service list (start order, watchdog) |
| `global_define.ini` | Service address map |
| `ls_dbagent/` | DB agent: SQL connection (`ls_dbagent_game.ini`, `ls_dbagent_log.ini`), query registry (`ls_query.ini`) |
| `ls_billingsvr/` | Billing server and billing relay configuration |
| `ls_mainsvr/` | Main server configuration (`config/*.ini`, `ls_auth.ini`) |
| `ls_gamesvr/` | Game server configuration (`config/*.ini`, mode tables in `config/tables/*.dat`) |
| `ls_loginsvr/` | Login server configuration |
| `ls_filewritesvr/` | File write server configuration |
| `ls_relaysvrSG/`, `ls_relaysvrTH/`, `ls_relaysvrHK/` | Regional relay configuration |

See [DATABASE.md](DATABASE.md) for database setup.

## Client side (data/client/)

Deploy by copying both folders into the client directory, next to the client
executable.

| Path | Purpose |
| --- | --- |
| `config/` | Client configuration and game data tables (`sp2*.ini`) |
| `xml/` | UI and definition XML files |

## Sanitization policy

The published files are sanitized. Entries that carried real credentials,
internal network addresses, or partner API keys were replaced with
placeholders (`CHANGE_ME`) or loopback defaults (`127.0.0.1`) — adjust them
per deployment:

- `ls_dbagent/ls_dbagent_game.ini`, `ls_dbagent_log.ini` — SQL `IP`, `ID`, `PW`
- `ls_billingsvr/` — internal relay IPs, `DaumApikey`, `NexonClientSecret`
- `ls_dbagent/ls_config_dba.ini` — `LogServerIP`
- `client/config/sp2.ini` — `log_server_ip`

`.githooks/pre-commit` blocks commits that reintroduce credentials, internal
network addresses, or local absolute paths anywhere under `data/`.
