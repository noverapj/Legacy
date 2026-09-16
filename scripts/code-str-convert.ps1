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
$DefPattern    = [regex]'^[\w\s:<>,*&]+\b([\w:~]+)\s*\('

$Keywords = @('if','else','for','while','switch','return','case','do','sizeof','new','delete','throw','try','catch','assert','sizeof')

# display call contexts - literals inside these convert
$AllowList = @(
    'SetMsgBox','AddTextPiece','SetTitleText','PrintText','PrintTextWidthCut',
    'sprintf','SafeSprintf','StringCbPrintf','wsprintf','SetText','SetComment',
    'AddChat','SetNotice','SetTitle\(','SetString','SetSubTitle','SetHelpText',
    'SetToolTip','SetDesc','AddTitle','SetMessage','SetRankText','SetStateText',
    'SetInfoText','SetGuideText','SetResultText','SetItemText','SetGradeText'
)
# internal contexts - literals may be identifiers, never convert
$DenyList = @(
    'strcmp','stricmp','strncmp','strnicmp','strstr','HashString\s*\(',
    'FindWnd','FindChildWnd','GetPrivateProfile','strlwr\s*\(','atoi\s*\('
)

function Read-StrictCp949 {
    param([string]$FilePath)
    try {
        $inBytes = [System.IO.File]::ReadAllBytes($FilePath)
        return $Enc.GetString($inBytes)
    } catch {
        return $null
    }
}

function Test-FileHasStrAccess {
    param([string]$Text)
    return ($Text -match 'STR\(') -or ($Text -match 'g_StringMgr') -or ($Text -match 'ioStringManager\.h')
}

# --- load existing exe_ keys: func -> max n ---
$funcMax = @{}
$entryKeys = [System.Collections.Generic.HashSet[string]]::new()
$tableLines = [System.Collections.Generic.List[string]]::new()
$tableContent = Read-StrictCp949 -FilePath $TextTable
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
        $s = Read-StrictCp949 -FilePath $f.FullName
        if ($null -eq $s) { continue }
        $lines = $s -split "`r`n"
        $ctx = $null
        for ($i = 0; $i -lt $lines.Count; $i++) {
            $line = $lines[$i]
            if ($line.Length -eq 0) { continue }
            if ($line[0] -ne ' ' -and $line[0] -ne "`t") {
                $dm = $DefPattern.Match($line)
                if ($dm.Success -and -not $line.TrimEnd().EndsWith(';')) {
                    $name = $dm.Groups[1].Value
                    if ($Keywords -notcontains $name) { $ctx = $name }
                }
            }
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
    $s = Read-StrictCp949 -FilePath $f.FullName
    if ($null -eq $s) { Write-Host "DECODE-FAIL: $($f.Name)"; $totals.decodeFails++; continue }

    $hasStrAccess = Test-FileHasStrAccess -Text $s

    $lines = $s -split "`r`n"
    $ctx = $null
    $ctxMax = @{}
    $ctxDedupe = @{}
    $fileSpans = [System.Collections.Generic.List[object]]::new()
    $fileEvents = [System.Collections.Generic.List[string]]::new()

    for ($i = 0; $i -lt $lines.Count; $i++) {
        $line = $lines[$i]
        if ($line.Length -eq 0) { continue }

        # function context tracking: col-0 definition lines
        if ($line[0] -ne ' ' -and $line[0] -ne "`t") {
            $dm = $DefPattern.Match($line)
            if ($dm.Success -and -not $line.TrimEnd().EndsWith(';')) {
                $name = $dm.Groups[1].Value
                if ($Keywords -notcontains $name) { $ctx = $name }
            }
            elseif ($line.TrimEnd() -eq '}') { $ctx = $null }
        }

        if (-not $KoreanLiteral.IsMatch($line)) { continue }

        $trimmed = $line.TrimStart()
        if ($trimmed.StartsWith('//') -or $trimmed.StartsWith('/*') -or $trimmed.StartsWith('*')) { $totals.comment++; continue }
        if ($line -match 'LOG\.') { $totals.log++; continue }

        # find all Korean literals on this line with positions
        $matches = $KoreanLiteral.Matches($line)
        if ($matches.Count -eq 0) { continue }

        # adjacent concat guard: a literal followed (spaces) by another quote
        $lineConcat = $false
        for ($k = 0; $k -lt $matches.Count; $k++) {
            $after = $line.Substring($matches[$k].Index + $matches[$k].Length)
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

        foreach ($m in $matches) {
            $val = $m.Groups[1].Value
            # escape guard: only \n and \r sequences allowed
            $stripped = $val -replace '\\n', '' -replace '\\r', ''
            if ($stripped.Contains('\')) { $totals.escape++; $fileEvents.Add("ESCAPE-SKIP line $($i+1): $val"); continue }

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
                        if ($l2.Length -gt 0 -and $l2[0] -ne ' ' -and $l2[0] -ne "`t") {
                            $dm2 = $DefPattern.Match($l2)
                            if ($dm2.Success -and -not $l2.TrimEnd().EndsWith(';')) {
                                $nm2 = $dm2.Groups[1].Value
                                if ($Keywords -notcontains $nm2) { $subCtx = $nm2 }
                            }
                            elseif ($l2.TrimEnd() -eq '}') { $subCtx = $null }
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

        $result = $outLines -join "`r`n"
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

        [System.IO.File]::WriteAllBytes($f.FullName, $Enc.GetBytes($result))
        $totals.files++
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
