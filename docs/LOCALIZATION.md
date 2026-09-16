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
