# Localization String Tables

Client strings resolve through `ioStringManager` (`src/io3DEngine/ioStringManager.cpp`)
at runtime. User-visible text can live in the text tables under
`data/client/resource/text/`:

- `text.txt` — Korean
- `text_en.txt` — English

Entry format: `|key|text|`, CP949 encoded. The loader lowercases keys and
decodes literal `\r` / `\n` into newlines. The `|` character cannot appear
inside a value.

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

- Converts only the five verified routes above; Korean elsewhere in the XML is left untouched
- Only values containing hangul are converted — everything else is byte-identical
- Per-file numbering continues after the highest existing `STR(n)`, so legacy references are never reassigned
- Strict CP949 round-trip per file; anything that fails to decode is skipped and reported
- Appends `|XML_<FileBase>_<n>|<text>|` entries to `text.txt` — `text_en.txt` is never touched

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

## Known gaps

- 13 multi-line texts (literal `\n` plus color markup) remain hardcoded — the
  table loader would decode their escapes, so they need a separate encoding
  decision before conversion
- Legacy dangling references with no entries (broken since the original
  build): `gashaponpresentlistwnd.xml` `STR(26)` and `gradeadjustmentwnd.xml`
  `STR(1,4-12)` — these windows currently display raw keys
- Legacy dangling ini references: mercenary buffs `219MB_SD_Stun`,
  `224MB_Jump_charge_Att_Air`, and `225MB_Dash_att03_defense` are
  referenced by weapon attributes but were never defined in any buff file
- `xml/xml/` is excluded by the converter (stale duplicate of two windows)
- English locale: `text_en.txt` has no `xml_` or `ini_` entries yet —
  converted content shows raw keys there until entries are added
