# Localization String Tables

Client strings resolve through `ioStringManager` (`src/io3DEngine/ioStringManager.cpp`)
at runtime. Tables are split per source and per locale under
`data/client/resource/text/{locale}/`:

| File | Prefix | Content |
|---|---|---|
| `app.txt` | `exe_` | Strings referenced from C++ code via the `STR(n)` macro |
| `ui.txt` | `xml_` | Strings referenced from client XML UI files |
| `config.txt` | `ini_` | Strings referenced from config ini files (items, quests, ...) |

Locale directories: `kr`, `us`, `jp`, `th`, `cn`, `tw`, `id`, `ph` (from
`ioLocalParent::GetTextDirName()`). There is **no fallback**: a missing file
leaves that table empty and unresolved keys display raw in the UI, plus a
`Load Failed` line in the client log — by design, so missing coverage is
immediately visible. Currently only `kr` (all three files) and `us`
(`app.txt`) are populated; the other locales need their own tables.

Entry format: `|key|text|`, CP949 encoded. The loader lowercases keys and
decodes literal `\r` / `\n` into newlines. The `|` character cannot appear
inside a value.

`scripts/split-text-table.ps1` performed the one-time migration from the
legacy flat `text.txt` / `text_en.txt` files (byte-preserving; the legacy
files used a `[vNNNN]` header and mixed LF/CRLF endings that the engine
tolerates). Remove the flat files after verifying the split at runtime.

## Key formats

| Prefix | Source | Format |
|---|---|---|
| `exe_` | C++ code — the `STR(n)` macro | `exe_<Class::Function>_<n>` |
| `xml_` | Client XML UI files | `xml_<XmlFileBaseName>_<n>` |
| `ini_` | Config files | `ini_<file>::<section>_<n>` |

## XML routes

`STR(n)` resolves in these XML positions (all verified against the engine):

1. `<Title><Text>` element content — `ioGUIManager.cpp` `OnTitleProperty`
2. `<Text><Text>` element content (text property) — `ioGUIManager.cpp` `OnTextProperty`
3. `<Tooltip><Help Text="...">` attribute — `ioGUIManager.cpp` `OnTooltipProperty`
4. `<ExtraInfo ...>` — all attribute values (the string manager is active while the window parses ExtraInfo)
5. `<TextBlock Text="...">` (rich label) — `ioRichLabel.cpp`
6. `<ExtraInfo><LabelInfo Text="...">` — `ioLabelWndEX` / `ioButtonWndEX`
   `_ParseExtraInfo` opt in via the `SetUseStringMgr` idiom (same pattern as
   `ioRichLabel`), with the key taken from the parent ExtraInfo element
7. `<TabTextN Text="...">` — `ioPowerUpManager::SetMenuList` (key is fixed
   to `myinventorywnd`)
8. `<NPC><LABEL NpcViewName / NpcGradeTitle>` in `npc_ai_npclist.xml` —
   `ioNpcMgr::BuildAdditionalNPCLabel` opts in with the key
   `npc_ai_npclist`

A `STR(n)` without a matching table entry displays the raw key in the UI —
keep entries and references in sync (`-Mode Verify` below).

## Converter tool

`scripts/xml-str-convert.ps1` (PowerShell 7):

```
pwsh scripts/xml-str-convert.ps1 -Mode Scan      # dry-run report
pwsh scripts/xml-str-convert.ps1 -Mode Convert   # rewrite XML + append text.txt
pwsh scripts/xml-str-convert.ps1 -Mode Verify    # cross-check refs vs entries
pwsh scripts/xml-str-convert.ps1 -Mode Convert -IncludeFile lobbywnd.xml,newshopwnd.xml
```

Rules:

- Converts only the verified routes above; Korean elsewhere in the XML is left untouched
- Only values containing hangul are converted — everything else is byte-identical
- Per-file numbering continues after the highest existing `STR(n)`, so legacy references are never reassigned
- Strict CP949 round-trip per file; anything that fails to decode is skipped and reported
- Appends `|XML_<FileBase>_<n>|<text>|` entries to `kr/ui.txt` — other
  locales are never touched

## Config converter tool

`scripts/ini-str-convert.ps1` (PowerShell 7) converts Korean values in
`data/client/config/` ini files the same way:

```
pwsh scripts/ini-str-convert.ps1 -Mode Scan|Convert|Verify
pwsh scripts/ini-str-convert.ps1 -Mode Verify   # folder-by-folder progress
```

- Scope: every ini under `data/client/config/` — the client loads all of
  them through `ioINILoader`, whose `LoadString` resolves `STR(n)` against
  the text table with key `ini_<FileBase>::<Section>_<n>`
- Numbering is per `(file, section)`; duplicate values inside one section
  share a number
- Files with a duplicated basename (difficulty variants under
  `monster_defence/` etc.) are converted once — the converter keeps the
  first and skips the rest with a `DUP-SKIP` report
- Values starting with a quote, or containing `|`, `STR(`, or `\r`/`\n`
  escapes are skipped and reported
- The server-side config copies never resolve `STR(n)` — the server
  loader has no string manager, so `data/server/` stays unconverted

`scripts/fix-mixed-encoding.ps1` normalizes ini files that contain stray
UTF-8 lines or BOMs into pure CP949 (strict round-trip, character-count
invariant enforced before any write).

## Fonts

Font selection is a per-locale virtual: `ioLocalParent::GetFontFileName()`
(default `lostsaga.ttf`), called from `ioApplication.cpp` after font manager
init. Font files live in `data/client/resource/font/` in this repo and are
loaded from `resource/Font/` in the runtime client directory.

| Locale | File | Font |
|---|---|---|
| Korea | `korea.ttf` | NanumGothic OTF (original client font) |
| Thailand | `thailand.ttf` | Noto Sans Thai |
| China | `chinese.ttf` | FZLanTingHeiS-DB1-GBK (remastered CN client font) |
| All others (default) | `lostsaga.ttf` | Noto Sans CJK KR Bold (universal: Hangul, Han S+T, Kana, Latin, Cyrillic) |

### Vertical layout normalization

UI text position is computed from the font's baseline ascent and line box
height (`ioFTFaceImpl::UpdateGlyphGlobalInfo`). Historically these came
from each font's own metrics, which shifted the whole UI whenever a font
with a different `max(hhea.ascender, head.yMax)` or bbox was loaded
(e.g. Noto Sans CJK `head.yMax` = 1806/1000 em vs NanumGothic 858).
The engine now uses fixed ratios — baseline ascent `0.858`, line box
height `1.081` of the pixel size — matching NanumGothic exactly, so any
font renders at identical positions. No per-font patching is required.

`scripts/patch-font-metrics.py` aligns a font's own tables to the same
ratios (head `-223/858`, hhea `800/-300`) for hygiene with external tools:

```
python scripts/patch-font-metrics.py --check <font.ttf>   # inspect metrics
python scripts/patch-font-metrics.py <font.ttf>            # patch in place
```

Pitfall: fontTools `recalcBBoxes=True` (the default) recomputes
`head.yMin/yMax` from the CFF FontBBox on save and silently reverts the
patch; the script sets it to False. Also note the client pipeline only
supports static OTF/TTF — variable fonts (`fvar`) are untested.

### Adding a font

1. Drop the file into `resource/Font/`
2. Add/override `GetFontFileName()` in the locale class (declaration in
   the header, definition in the cpp)
3. Optionally verify metrics with `--check`; patch if you want clean tables
4. Check the license — `OFL.txt` must ship alongside OFL fonts.
   `chinese.ttf` (FZLanTingHei) is a commercial font inherited from the
   original CN client — a future swap to Noto Sans SC would remove that
   licensing concern

## Name pipeline (cross-locale identities)

Player-facing identity strings — nicknames (PublicID), guild names,
tournament/team names — follow a **canonical UTF-8** design so a single
global-community server works: the name is created as UTF-8, stored as
UTF-8, transmitted as UTF-8, and converted to UTF-16 **only at the render
boundary**. Names are lookup keys (`ioHashString`), so there is no
conversion at packet boundaries at all — bytes round-trip exactly.

```
input (native CP949/874/936...)
  --Help::ToWire()--> UTF-8 canonical --raw--> packets/DB/render cache
                                                   |
                     Help::NameToWide() --> UTF-16 wide draw (glyph fallback)
```

### Input conversion (native → UTF-8)

- Creation: `FirstIDChangeWnd`, `IDChangeWnd`/`ChangeNameWnd` (nickname and
  guild-name change items), guild creation (`GuildWnd`), tournament and
  team creation — validation runs on the native input first, then
  `Help::ToWire()` at the packet
- Typed target inputs: memo recipient, friend application, channel
  invite — `ToWire()` at the packet
- Prefilled memo target: `MemoListWnd` stashes the original UTF-8 name
  (`m_szOriginalTargetID`) and renders it losslessly via
  `ioEdit::SetTextWide`; if the user does not modify the prefilled
  edit text the **original bytes** are sent (byte-exact cross-charset
  reply); `ioMemoManager::SendMemo` takes an optional `szWireFromID`
  for that path
- Names that arrive from the server (friend/guild/tournament lists,
  kill feed) are already UTF-8 and are echoed back **raw** — never
  re-converted

### Wire budget (length policy)

The native UX gate stays `ID_NUMBER` (20 native bytes = 10 Hangul chars,
unchanged); the **wire form** of the same name is 3 bytes per Hangul
char, so all wire-side buffers use a separate budget:

| Macro | Non-TH | TH build |
|---|---|---|
| `ID_NUMBER_WIRE` / `ID_NUM_WIRE_PLUS_ONE` | 30 / 31 | 60 / 61 |

Defined in the client (`NoveraClient/GameEnumType.h`) and in every
server `Define.h` (gamesvr, billingsvr, mainsvr, filewritesvr);
`ls_relaysvr` mirrors it as `PUBLICID_MAX 31`. The relay join/leave
structs carry `m_szPublicID[31]` in gamesvr and filewritesvr — **all
server binaries must be rebuilt together** (layout change 21→31).

The server acceptance gate (`ioMainProcess::IsRightID` in gamesvr)
validates against `ID_NUMBER_WIRE` and walks **UTF-8 sequences**
(rejects broken/half characters). `IsNotMakeID` matches with byte-level
`strstr`.

### Display conversion (UTF-8 → wide render)

- `Help::NameToWide()` — UTF-8 → UTF-16, with legacy single-charset
  data falling back to runtime-codepage decode
- Engine wide render path: `ioFontManager::PrintTextWide` /
  `PrintTextWidthCutWide`, `GetTextWidthWide` (2 and 4-param),
  `GetTextWidthCutSizeWide`, `ioFontWorkSpace::GetTextPieceWide` +
  `CalculateTextWidthWide`, `ioUITitle::SetTextWide` /
  `ioWnd::SetTitleTextWide`, label macro `SetLabelTextWide`
- Composites (chat sender prefix, system messages with names):
  `ioComplexStringPrinter::AddTextPieceWide` pieces —
  `Help::FormatWide` / `FormatWide2` / `FormatWideFromWide` +
  `Help::WideStringCut` (mirror of `StringCutFun`)
- **Font fallback chain**: every loaded font gets size-matched fallback
  faces loaded from `lostsaga.ttf` (Noto CJK: Hangul, Han, Kana) and
  `thailand.ttf` (Noto Thai); a missing glyph (FT char index 0) falls
  through to the next face, so a KR client renders Thai names and a TH
  client renders Korean names
- Covered surfaces: world nameplate (`ioBaseGUISupport` normal +
  observer, the six dummy-char copies), in-game round list, observer
  lists, result screens, party/plaza/ladder/HQ/house/trade/tournament
  rosters, hero history, present list, chat (7 chat types + join/leave/
  kick/score/flag/goal messages), manner window, kick vote, memo
  windows, guild invite

### Server + DB notes

- ~300 server-side name/ID buffer bindings widened to the wire budget
  (gamesvr DB layer, UserNodeManager, mainsvr DB layer, tournament,
  trade, ranking)
- DB schema (all 20 files in `sql/`, including `linux/` and `windows/`
  variants): every nickname column and stored-proc parameter widened
  `varchar(20) -> varchar(30)` **and converted `nvarchar`/`nchar` →
  `varchar`** — UTF-8 names are raw bytes; the nvarchar driver
  conversion path would re-encode and corrupt them. Log DBs included.
  The `lite/` SQLite variant has no nickname columns.

### Test matrix

| Case | Expected |
|---|---|
| Create KR nickname on KR client | 10 Hangul chars accepted (native gate), stored/echoed as 30 UTF-8 bytes |
| Create TH nickname on TH client | Thai chars accepted, wire bytes ≤ 30 |
| KR player in room, TH client | Nameplate + lists render Hangul via Noto CJK fallback |
| TH player, KR client | Name renders via Noto Thai fallback (or `?` only if fallback font missing) |
| Chat sender cross-locale | Sender prefix renders in sender script; chat text as before (F2 wire rules) |
| Whisper/memo reply to cross-charset name | Reply reaches target byte-exact (shadow buffer) |
| Rename while in-room | Guild/friend/memo lists re-key on the echoed name; relay structs 31-byte |
| Guild/tournament/team names | Creation validation native, display wide, DB varchar(30) |

## Known gaps

- 13 multi-line texts (literal `\n` plus color markup) remain hardcoded — the
  table loader would decode their escapes, so they need a separate encoding
  decision before conversion
- Shop tab labels in `shopwnd.xml` containing a literal `|`
  (금주의 상품, 신규, 인기, 용병, 장비, 보급, 치장, 모션, 특별, 블럭, 클로버샵)
  cannot be converted — the `|key|text|` table format has no escaping, so
  these stay raw Korean until the entry format grows an escape mechanism
- Legacy dangling references with no entries (broken since the original
  build): `gashaponpresentlistwnd.xml` `STR(26)` and `gradeadjustmentwnd.xml`
  `STR(1,4-12)` — these windows currently display raw keys
- ~181 dangling `ini_` refs (98 sections) across champion_ai skill/buff
  descriptions, field NPC practice AI, inventory manuals, extra item
  machine sections, field NPC names, and two quest help sections.
  These refs exist **identically in the original Korean 2020 client
  shipping** (verified against the raw shipping config): the original
  developers' build wrote the refs but never shipped the table entries.
  The Korean text for them exists nowhere in the available sources
  (shipping raw, base source, and latest KR tables all lack it), so they
  display raw keys — the same state as the original live Korean client.
- ~245 dangling `exe_` refs (pre-existing dev STR(n) calls whose entries
  were never shipped in the original exe_ table): EtcHelpFunc
  `GetRemainTime`/`GetGrowthTitle`, ioUserEtcItem, NewShopBuyWnd,
  ioEtcItem, PracticeToolTip, ioBaseChar, ioNProtect, and others.
  Same phenomenon as the ini_ dangling refs — the dev's original build
  wrote the refs but the entries are absent from the shipped table, so
  those UI slots display raw keys in the original client too.
- Legacy dangling ini references: mercenary buffs `219MB_SD_Stun`,
  `224MB_Jump_charge_Att_Air`, and `225MB_Dash_att03_defense` are
  referenced by weapon attributes but were never defined in any buff file
- `xml/xml/` is excluded by the converter (stale duplicate of two windows)
- English locale: `us/` only has `app.txt` — `xml_` and `ini_` content
  shows raw keys there until `us/ui.txt` and `us/config.txt` are filled
- Room titles are player-typed but were not converted at input yet
  (F5 covered identity names only) — cross-locale room titles still
  render in the creator's charset; needs a follow-up input sweep
- Edit boxes display native text while **editing** (the IME input box is the
  live native source); a prefilled cross-charset name renders losslessly via
  `ioEdit::SetTextWide` when the edit is not focused (the **send** is
  byte-exact via the shadow buffer). Typing a non-native-charset script
  (e.g. Thai on a KR-codepage client) still requires the wide IME input
  chain — a possible future phase beyond F6
- Banned-word / not-make-ID lists are CP949-encoded data files; they
  will not match UTF-8 names until UTF-8 versions of the lists are
  shipped (data follow-up, no code change needed)
- Account `userID varchar(12)` columns are out of scope of the name
  pipeline (separate identity, separate policy)
- Untraced display windows kept narrow pending verification:
  `ioBingoRewardWnd`, `EnablePowerupListWnd`, `ioExtraItemListWnd`
  (callers not found — likely rare/unused)
- `ioSP2ChatManager::SendChatLog` writes a mixed-encoding log line
  (UTF-8 name + native chat text) to the log server as raw bytes
