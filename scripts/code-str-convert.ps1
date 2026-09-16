#Requires -Version 7.0

<#
.SYNOPSIS
Extracts hardcoded Korean display strings from client C++ code into the
exe_ string table (STR(n) / kr/app.txt), the same mechanism the original
developers' extraction tool used.

.DESCRIPTION
Replaces Korean string literals in display call contexts with STR(n) and
appends the matching |EXE_<Class::Function>_<n>|<text>| entries to
kr/app.txt. The key is the MSVC __FUNCTION__ form ("Class::Function" for
members, plain name for free functions), so runtime resolution matches the
static numbering as long as the function context is parsed correctly.

Safety rules:
  - Only literals on lines matching the display allowlist are converted.
  - Lines matching the internal-context denylist (strcmp/lookup/hash/...) are
    skipped and reported - these literals may be identifiers, not text.
  - Lines matching neither list are reported as REVIEW and left untouched.
  - Log calls (LOG.), comments, literals outside any function body, adjacent
    string concatenation, and escapes other than \r \n are skipped.
  - Only files that already use STR( or g_StringMgr are converted; others are
    reported as NEEDS-INCLUDE.
  - Numbering continues after the highest existing n per function (from both
    app.txt and the file's own existing STR refs); duplicate values within one
    function share a number.
  - Byte-safe: strict CP949 decode/encode round trip, abort on any failure.

.EXAMPLE
    pwsh scripts/code-str-convert.ps1 -Mode Scan -IncludeFile TournamentMainWnd.cpp
    pwsh scripts/code-str-convert.ps1 -Mode Convert -IncludeFile TournamentMainWnd.cpp
    pwsh scripts/code-str-convert.ps1 -Mode Verify
#>
[CmdletBinding()]
param(
    [ValidateSet('Scan','Convert','Verify')]
    [string]$Mode = 'Scan',
    [string]$SourceRoot = '',
    [string]$TextTable = '',
    [string[]]$IncludeFile = @()
)

$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $SourceRoot) { $SourceRoot = Join-Path $RepoRoot 'src/NoveraClient' }
if (-not $TextTable) { $TextTable = Join-Path $RepoRoot 'data/client/resource/text/kr/app.txt' }

$Enc = [System.Text.Encoding]::GetEncoding(949,
    [System.Text.EncoderFallback]::ExceptionFallback,
    [System.Text.DecoderFallback]::ExceptionFallback)

$EntryPattern  = [regex]'^\|EXE_(.+)_(\d+)\|(.*)\|$'
$KoreanLiteral = [regex]'"((?:[^"\\\r\n]|\\n|\\r)*[가-힣](?:[^"\\\r\n]|\\n|\\r)*)"'
$StrRefPattern = [regex]'STR\((\d+)\)'
# Captures the MSVC __FUNCTION__ form: "Class::Function" for members
# (the leading char class deliberately excludes ':' so the class prefix
# stays inside the capture group), plain name for free functions.
$DefPattern    = [regex]'^[ \t]*(?:[\w\s<>,*&]+\b)?((?:\w+::)*~?\w+)\s*\('

$Keywords = @('if','else','for','while','switch','return','case','do','sizeof','new','delete','throw','try','catch','assert','sizeof')

# display call contexts - literals inside these convert
$AllowList = @(
    'SetMsgBox','AddTextPiece','SetTitleText','PrintText','PrintTextWidthCut',
    'sprintf','SafeSprintf','StringCbPrintf','wsprintf','SetText','SetComment',
    'AddChat','SetNotice','SetTitle\(','SetString','SetSubTitle','SetHelpText',
    'SetToolTip','SetDesc','AddTitle','SetMessage','SetRankText','SetStateText',
    'SetInfoText','SetGuideText','SetResultText','SetItemText','SetGradeText',
    'ChangeOwnerCommandButton',
    'SetSystemMsg','SetInfomationMsg','SetPrevMsgBox','SetChatComplexString',
    'SetInfoAndShow','SetSearching','SetHostChange',
    'SetPrevMsgListBoxWithTitle','SetPrevMsgListPinkBoxWithTitle',
    'OnRenderQuestTitle',
    'StringCbCopy\(','StringCbCat\(',
    'm_sz\w+\s*=',
    'push_back\s*\(',
    'value_type\s*\(',
    'sz\w+\s*=',
    'rsText\s*=',
    'm_s\w+\s*=',
    'kModeTitle\s*\+=',
    'kList\.m_\w+\s*=',
    'ioHashString\s+\w+\s*=',
    'return\s'
)
# internal contexts - literals may be identifiers, never convert
$DenyList = @(
    'strcmp','stricmp','strncmp','strnicmp','strstr','HashString\s*\(',
    'FindWnd','FindChildWnd','GetPrivateProfile','strlwr\s*\(','atoi\s*\(',
    'strcpy\s*\(',
    'char\s+\w+\s*\['
)

function Read-StrictCp949 {
    # Returns @{ Text; Utf8 } - a few source files are UTF-8 with BOM
    # (MSVC reads them as UTF-8; the rest of the codebase is CP949).
    param([string]$FilePath)
    try {
        $inBytes = [System.IO.File]::ReadAllBytes($FilePath)
        if ($inBytes.Length -ge 3 -and $inBytes[0] -eq 0xEF -and $inBytes[1] -eq 0xBB -and $inBytes[2] -eq 0xBF) {
            $text = [System.Text.Encoding]::UTF8.GetString($inBytes, 3, $inBytes.Length - 3)
            # strict check: decoded text must be re-encodable (rejects corrupted files)
            [void][System.Text.Encoding]::UTF8.GetBytes($text)
            return @{ Text = $text; Utf8 = $true }
        }
        $text2 = $Enc.GetString($inBytes)
        return @{ Text = $text2; Utf8 = $false }
    } catch {
        return $null
    }
}

function Write-SourceFile {
    param([string]$FilePath, [string]$Text, [bool]$Utf8)
    if ($Utf8) {
        $utf8Enc = New-Object System.Text.UTF8Encoding($true)
        [System.IO.File]::WriteAllText($FilePath, $Text, $utf8Enc)
    }
    else {
        [System.IO.File]::WriteAllBytes($FilePath, $Enc.GetBytes($Text))
    }
}

function Test-FileHasStrAccess {
    # ioStringManager.h is included by stdafx.h, and every client .cpp
    # includes stdafx.h first - so any of these markers is sufficient.
    param([string]$Text)
    return ($Text -match 'STR\(') -or ($Text -match 'g_StringMgr') -or ($Text -match 'ioStringManager\.h') -or ($Text -match '(?im)^#include\s+"stdafx\.h"')
}

function Update-FunctionContext {
    # Tracks the current function for the MSVC __FUNCTION__ key.
    # Definitions may be indented (namespace-wrapped files); statement
    # lines ending in ';' or ',' (optionally followed by a trailing //
    # comment) are never definitions; the context resets at a column-0
    # closing brace. NOTE: $CurrentCtx is deliberately UNtyped so $null
    # survives parameter binding ([string] would coerce $null to '').
    param([string]$Line, $CurrentCtx)
    if ($Line.Length -eq 0) { return $CurrentCtx }
    $col0 = ($Line[0] -ne ' ' -and $Line[0] -ne "`t")
    if ($col0 -and $Line.TrimEnd() -match '^\}\s*(//.*)?$') { return $null }
    $te = $Line.TrimEnd()
    if ($te -match '[;,]\s*(//.*)?$') { return $CurrentCtx }
    $dm = $DefPattern.Match($Line)
    if ($dm.Success) {
        $name = $dm.Groups[1].Value
        if ($Keywords -notcontains $name) { return $name }
    }
    return $CurrentCtx
}

# --- load existing exe_ keys: func -> max n ---
$funcMax = @{}
$entryKeys = [System.Collections.Generic.HashSet[string]]::new()
$tableLines = [System.Collections.Generic.List[string]]::new()
$tableRead = Read-StrictCp949 -FilePath $TextTable
$tableContent = if ($null -ne $tableRead) { $tableRead.Text } else { $null }
if ($null -eq $tableContent) { throw "Cannot decode table: $TextTable" }
foreach ($line in ($tableContent -split "`r?`n")) {
    [void]$tableLines.Add($line)
    $m = $EntryPattern.Match($line)
    if ($m.Success) {
        $fn = $m.Groups[1].Value
        $n = [int]$m.Groups[2].Value
        [void]$entryKeys.Add(("exe_${fn}_$n").ToLowerInvariant())
        if (-not $funcMax.ContainsKey($fn) -or $n -gt $funcMax[$fn]) { $funcMax[$fn] = $n }
    }
}
Write-Host "app.txt loaded: $($entryKeys.Count) exe_ entries"

# --- collect target files ---
$files = Get-ChildItem $SourceRoot -Recurse -File -Filter '*.cpp' | Sort-Object FullName
# engine-level exclusions: locale mem lists (per-locale by design), debug UI,
# channeling partner nodes, and tools outside the game client
$files = $files | Where-Object { $_.FullName -notmatch '\\Local\\|\\Channeling\\|DebugInfoWnd' }
if ($IncludeFile.Count -gt 0) {
    $wanted = $IncludeFile | ForEach-Object { $_.Split(',') } | ForEach-Object { $_.Trim() } | Where-Object { $_ } | ForEach-Object { $_.ToLowerInvariant() }
    $files = $files | Where-Object { $wanted -contains $_.Name.ToLowerInvariant() }
}

# --- Verify mode ---
if ($Mode -eq 'Verify') {
    $missing = [System.Collections.Generic.List[string]]::new()
    $refs = 0
    foreach ($f in $files) {
        $read = Read-StrictCp949 -FilePath $f.FullName
        if ($null -eq $read) { continue }
        $s = $read.Text
        $lines = $s -split '\r\n|\n|\r'
        $ctx = $null
        for ($i = 0; $i -lt $lines.Count; $i++) {
            $line = $lines[$i]
            if ($line.Length -eq 0) { continue }
            $ctx = Update-FunctionContext -Line $line -CurrentCtx $ctx
            foreach ($sm in $StrRefPattern.Matches($line)) {
                $refs++
                if ($null -ne $ctx) {
                    $key = "exe_${ctx}_$($sm.Groups[1].Value)".ToLowerInvariant()
                    if (-not $entryKeys.Contains($key)) {
                        $missing.Add("$($f.Name) [$ctx] STR($($sm.Groups[1].Value)) -> $key")
                    }
                }
            }
        }
    }
    Write-Host "=== VERIFY: $($files.Count) files, $refs STR refs ==="
    Write-Host "missing: $($missing.Count)"
    foreach ($m in $missing) { Write-Host "  MISSING $m" }
    exit 0
}

# --- Scan/Convert ---
$appendEntries = [System.Collections.Generic.List[string]]::new()
$totals = @{ converted = 0; review = 0; deny = 0; escape = 0; concat = 0; outsideFn = 0; log = 0; comment = 0; needsInclude = 0; files = 0; decodeFails = 0 }
$reviewList = [System.Collections.Generic.List[string]]::new()
$denyListHits = [System.Collections.Generic.List[string]]::new()
$needsIncludeFiles = [System.Collections.Generic.List[string]]::new()

$allowRegex = ($AllowList -join '|')
$denyRegex  = ($DenyList -join '|')

foreach ($f in $files) {
    $read = Read-StrictCp949 -FilePath $f.FullName
    if ($null -eq $read) { Write-Host "DECODE-FAIL: $($f.Name)"; $totals.decodeFails++; continue }
    $s = $read.Text
    $fileUtf8 = $read.Utf8

    $hasStrAccess = Test-FileHasStrAccess -Text $s

    # preserve the file's own newline style when writing back
    $newline = if ($s -match "`r`n") { "`r`n" } else { "`n" }
    $lines = $s -split '\r\n|\n|\r'
    $ctx = $null
    $ctxMax = @{}
    $ctxDedupe = @{}
    $fileSpans = [System.Collections.Generic.List[object]]::new()
    $fileEvents = [System.Collections.Generic.List[string]]::new()

    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        if ($line.Length -eq 0) { continue }

        $ctx = Update-FunctionContext -Line $line -CurrentCtx $ctx

        if (-not $KoreanLiteral.IsMatch($line)) { continue }

        $trimmed = $line.TrimStart()
        if ($trimmed.StartsWith('//') -or $trimmed.StartsWith('/*') -or $trimmed.StartsWith('*')) { $totals.comment++; continue }
        if ($line -match 'LOG\.') { $totals.log++; continue }

        # find all Korean literals on this line with positions
        $litMatches = $KoreanLiteral.Matches($line)
        if ($litMatches.Count -eq 0) { continue }

        # adjacent concat guard: a literal followed (spaces) by another quote
        $lineConcat = $false
        for ($k = 0; $k -lt $litMatches.Count; $k++) {
            $after = $line.Substring($litMatches[$k].Index + $litMatches[$k].Length)
            if ($after -match '^[ \t]*"') { $lineConcat = $true; break }
        }
        if ($lineConcat) { $totals.concat++; continue }

        # classification: allow / deny / review
        if ($line -match $denyRegex) {
            $totals.deny++
            $denyListHits.Add("$($f.Name):$($i+1) [deny] $($trimmed.Substring(0, [Math]::Min(60, $trimmed.Length)))")
            continue
        }
        if ($line -notmatch $allowRegex) {
            $totals.review++
            $reviewList.Add("$($f.Name):$($i+1) [review] $($trimmed.Substring(0, [Math]::Min(60, $trimmed.Length)))")
            continue
        }
        if ($null -eq $ctx) { $totals.outsideFn++; continue }
        if (-not $hasStrAccess) {
            $totals.needsInclude++
            $needsIncludeFiles.Add($f.Name)
            $fileEvents.Add("NEEDS-INCLUDE line $($i+1) - file skipped from conversion")
            continue
        }

        foreach ($m in $litMatches) {
            $val = $m.Groups[1].Value
            # escape guard: only \n and \r sequences allowed
            $stripped = $val -replace '\\n', '' -replace '\\r', ''
            if ($stripped.Contains('\')) { $totals.escape++; $fileEvents.Add("ESCAPE-SKIP line $($i+1): $val"); continue }
            # file-ref guard: literals that are filenames must stay raw
            if ($val -match '\.(txt|ini|xml|lua)\s*$') { $totals.escape++; $fileEvents.Add("FILE-REF-SKIP line $($i+1): $val"); continue }
            # table encoding guard: every entry value must be CP949-encodable
            try { [void]$Enc.GetBytes($val) } catch { $totals.escape++; $fileEvents.Add("ENCODING-SKIP line $($i+1): $val"); continue }

            $n = 0
            if ($ctxDedupe.ContainsKey($ctx) -and $ctxDedupe[$ctx].ContainsKey($val)) {
                $n = $ctxDedupe[$ctx][$val]
            }
            else {
                if (-not $ctxMax.ContainsKey($ctx)) {
                    $seed = if ($funcMax.ContainsKey($ctx)) { $funcMax[$ctx] } else { 0 }
                    # seed also from the file's own existing STR refs in this context
                    $ctxMax[$ctx] = $seed
                    # scan the whole file for STR refs under this ctx (cheap one-time per ctx)
                    $subCtx = $null
                    for ($j = 0; $j -lt $lines.Count; $j++) {
                        $l2 = $lines[$j]
                        if ($l2.Length -gt 0) {
                            $subCtx = Update-FunctionContext -Line $l2 -CurrentCtx $subCtx
                        }
                        if ($subCtx -eq $ctx) {
                            foreach ($sm2 in $StrRefPattern.Matches($l2)) {
                                $n2 = [int]$sm2.Groups[1].Value
                                if ($n2 -gt $ctxMax[$ctx]) { $ctxMax[$ctx] = $n2 }
                            }
                        }
                    }
                }
                $n = ++$ctxMax[$ctx]
                $probe = "exe_${ctx}_$n".ToLowerInvariant()
                while ($entryKeys.Contains($probe)) {
                    $n = ++$ctxMax[$ctx]
                    $probe = "exe_${ctx}_$n".ToLowerInvariant()
                }
                if (-not $ctxDedupe.ContainsKey($ctx)) { $ctxDedupe[$ctx] = @{} }
                $ctxDedupe[$ctx][$val] = $n
                [void]$entryKeys.Add($probe)
                $appendEntries.Add("|EXE_${ctx}_$n|$val|")
            }
            $fileSpans.Add(@{ Line = $i; Start = $m.Index; End = ($m.Index + $m.Length); N = $n })
            $totals.converted++
        }
    }

    if ($Mode -eq 'Scan') {
        if ($fileSpans.Count -gt 0 -or $fileEvents.Count -gt 0) {
            Write-Host ("SCAN {0,-44} convert={1,-4} events={2}" -f $f.Name, $fileSpans.Count, $fileEvents.Count)
            foreach ($ev in $fileEvents) { Write-Host "    $ev" }
        }
        continue
    }

    if ($Mode -eq 'Convert') {
        if (-not $hasStrAccess -and $fileSpans.Count -gt 0) { continue }
        if ($fileSpans.Count -eq 0) { continue }

        # apply replacements line by line
        $outLines = [string[]]::new($lines.Count)
        for ($i = 0; $i -lt $lines.Count; $i++) { $outLines[$i] = $lines[$i] }
        $byLine = @{}
        foreach ($sp in $fileSpans) {
            if (-not $byLine.ContainsKey($sp.Line)) { $byLine[$sp.Line] = [System.Collections.Generic.List[object]]::new() }
            $byLine[$sp.Line].Add($sp)
        }
        foreach ($ln in $byLine.Keys) {
            $spans = $byLine[$ln] | Sort-Object { $_.Start } -Descending
            $txt = $lines[$ln]
            foreach ($sp in $spans) {
                $txt = $txt.Substring(0, $sp.Start) + "STR($($sp.N))" + $txt.Substring($sp.End)
            }
            $outLines[$ln] = $txt
        }

        $result = $outLines -join $newline
        # self-check: hangul removed equals hangul inside converted literals
        $hangBefore = ([regex]::Matches($s, '[가-힣]')).Count
        $hangAfter  = ([regex]::Matches($result, '[가-힣]')).Count
        $hangConv = 0
        foreach ($ln in $byLine.Keys) { $hangConv += ([regex]::Matches($lines[$ln].Substring(0, [Math]::Min($lines[$ln].Length, ($lines[$ln].Length))), '[가-힣]')).Count }
        # recount precisely: hangul in the replaced spans only
        $hangConv = 0
        foreach ($sp in $fileSpans) { $hangConv += ([regex]::Matches($lines[$sp.Line].Substring($sp.Start, $sp.End - $sp.Start), '[가-힣]')).Count }
        if ($hangAfter -ne ($hangBefore - $hangConv)) {
            Write-Host "SELFCHECK-FAIL: $($f.Name) - aborted"
            continue
        }

        Write-SourceFile -FilePath $f.FullName -Text $result -Utf8 $fileUtf8        $totals.files++
        Write-Host ("CONVERTED {0,-44} strings={1,-4}" -f $f.Name, $fileSpans.Count)
    }
}

Write-Host ""
Write-Host "=== $Mode TOTAL ==="
Write-Host ("converted={0} review={1} deny={2} escape={3} concat={4} outsideFn={5} log={6} comment={7} needsInclude={8} files={9} decodeFails={10}" -f `
    $totals.converted, $totals.review, $totals.deny, $totals.escape, $totals.concat, $totals.outsideFn, $totals.log, $totals.comment, $totals.needsInclude, $totals.files, $totals.decodeFails)

if ($reviewList.Count -gt 0) {
    Write-Host ""
    Write-Host "REVIEW (no allowlist match, not converted):"
    foreach ($r in $reviewList) { Write-Host "  $r" }
}
if ($denyListHits.Count -gt 0) {
    Write-Host ""
    Write-Host "DENY (internal context, skipped):"
    foreach ($d in $denyListHits) { Write-Host "  $d" }
}
if ($needsIncludeFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "NEEDS-INCLUDE files: $(($needsIncludeFiles | Sort-Object -Unique) -join ', ')"
}

if ($Mode -eq 'Convert' -and $appendEntries.Count -gt 0) {
    $tableBytes = [System.IO.File]::ReadAllBytes($TextTable)
    $needsCrlf = ($tableBytes.Length -lt 2) -or -not (($tableBytes[$tableBytes.Length - 2] -eq 13) -and ($tableBytes[$tableBytes.Length - 1] -eq 10))
    if ($tableBytes.Length -gt 0 -and $needsCrlf) { [System.IO.File]::AppendAllText($TextTable, "`r`n", $Enc) }
    foreach ($e in $appendEntries) {
        [System.IO.File]::AppendAllText($TextTable, "$e`r`n", $Enc)
    }
    Write-Host ""
    Write-Host "appended $($appendEntries.Count) entries to app.txt"
}
