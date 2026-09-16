#Requires -Version 7.0

<#
.SYNOPSIS
Rebuilds dangling ini_ string refs by restoring raw sections from the
original client shipping config and letting ini-str-convert reconvert them.

.DESCRIPTION
Sections whose STR(n) refs have no table entry (dangling refs from the
pre-repo conversion era) are fixed by restoring:

  1. Replace the affected section content in the current config file with
     the RAW section from the original shipping config (byte-preserving
     Latin-1 splice).
  2. Delete that section's entries (INI_<base>::<section>_<n>) from
     kr/config.txt.
  3. (manually) run ini-str-convert -Mode Convert -IncludeFile <files> to
     reconvert the restored raw values with fresh, consistent numbering.

Sections that are already STR-converted in the original shipping as well
(champion_ai, fieldnpc/Practice_NPC) have no raw source anywhere - they are
reported as NO-RAW and left untouched (matches the original live client).

All file splicing is done via Latin-1 (byte-preserving) reads/writes so the
CP949 content is never re-encoded.

.EXAMPLE
    pwsh scripts/fix-dangling-ini-refs.ps1 -Mode Scan
    pwsh scripts/fix-dangling-ini-refs.ps1 -Mode Fix
#>
[CmdletBinding()]
param(
    [ValidateSet('Scan','Fix')]
    [string]$Mode = 'Scan',
    [string]$ConfigDir = '',
    [string]$TextTable = '',
    [string]$OrigConfig = ''
)

$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $ConfigDir) { $ConfigDir = Join-Path $RepoRoot 'data/client/config' }
if (-not $TextTable) { $TextTable = Join-Path $RepoRoot 'data/client/resource/text/kr/config.txt' }
if (-not $OrigConfig) { $OrigConfig = 'C:\Users\LSDH\AppData\Local\Temp\opencode\orig_config\config' }

$Cp949 = [System.Text.Encoding]::GetEncoding(949)
$Latin1 = [System.Text.Encoding]::GetEncoding(28591)

$SectionPattern = [regex]'(?m)^\[([^\]\r\n]+)\]'
$StrPattern = [regex]'STR\((\d+)\)'
$Hangeul = [regex]'[가-힣]'

function Read-Latin1 {
    param([string]$FilePath)
    return $Latin1.GetString([System.IO.File]::ReadAllBytes($FilePath))
}

function Write-Latin1 {
    param([string]$FilePath, [string]$Text)
    [System.IO.File]::WriteAllBytes($FilePath, $Latin1.GetBytes($Text))
}

function Find-Section {
    # returns @{ Start; End } char indexes (Latin-1 space) of a section body
    # (from its '[' up to the next section '[' or EOF)
    param([string]$Text, [string]$Section)
    $escaped = [regex]::Escape($Section)
    $startMatch = [regex]::Match($Text, "(?m)^\[$escaped\]")
    if (-not $startMatch.Success) { return $null }
    $start = $startMatch.Index
    $nextMatch = [regex]::Match($Text.Substring($startMatch.Index + $startMatch.Length), '(?m)^\[')
    $end = if ($nextMatch.Success) { $startMatch.Index + $startMatch.Length + $nextMatch.Index } else { $Text.Length }
    return @{ Start = $start; End = $end }
}

# --- collect dangling (file, section) pairs ---
$tableText = Read-Latin1 -FilePath $TextTable
$tableKeys = [System.Collections.Generic.HashSet[string]]::new()
foreach ($m in [regex]::Matches($tableText, '(?m)^\|([^|]+)\|')) {
    [void]$tableKeys.Add($m.Groups[1].Value.ToLowerInvariant())
}

$files = Get-ChildItem $ConfigDir -Recurse -File -Filter '*.ini' | Sort-Object FullName
$dangling = [System.Collections.Generic.List[object]]::new()

foreach ($f in $files) {
    $rel = $f.FullName.Substring($ConfigDir.Length + 1)
    $cur = Read-Latin1 -FilePath $f.FullName
    $strMatches = [regex]::Matches($cur, 'STR\((\d+)\)')
    if ($strMatches.Count -eq 0) { continue }
    $base = $f.BaseName.ToLowerInvariant()

    # section index map for the current file (CP949 section names are ASCII-safe,
    # but resolve via Latin-1 indexes)
    $secMatches = $SectionPattern.Matches($cur)
    $secPos = [int[]]::new($secMatches.Count)
    $secName = [string[]]::new($secMatches.Count)
    for ($i = 0; $i -lt $secMatches.Count; $i++) {
        $secPos[$i] = $secMatches[$i].Index
        $secName[$i] = $secMatches[$i].Groups[1].Value.Trim()
    }

    $seenSections = [System.Collections.Generic.HashSet[string]]::new()
    foreach ($sm in $strMatches) {
        $n = $sm.Groups[1].Value
        # resolve section of this ref
        $sec = ''
        $si = -1
        for ($i = 0; $i -lt $secPos.Count; $i++) {
            if ($secPos[$i] -lt $sm.Index) { $si = $i } else { break }
        }
        if ($si -ge 0) { $sec = $secName[$si] }
        if ($sec -eq '') { continue }
        $key = "ini_${base}::${sec}_${n}".ToLowerInvariant()
        if (-not $tableKeys.Contains($key)) {
            [void]$seenSections.Add($sec)
        }
    }
    foreach ($sec in $seenSections) {
        $dangling.Add(@{ Rel = $rel; Base = $base; Section = $sec })
    }
}

Write-Host "dangling sections found: $($dangling.Count)"

# --- classify: has raw source? ---
$classA = [System.Collections.Generic.List[object]]::new()
$classB = [System.Collections.Generic.List[string]]::new()

foreach ($d in $dangling) {
    $origPath = Join-Path $OrigConfig (($d.Rel) -replace '/', '\')
    if (-not (Test-Path -LiteralPath $origPath)) {
        $classB.Add("$($d.Rel) [$($d.Section)] (no raw file)")
        continue
    }
    $orig = Read-Latin1 -FilePath $origPath
    $span = Find-Section -Text $orig -Section $d.Section
    if ($null -eq $span) {
        $classB.Add("$($d.Rel) [$($d.Section)] (section absent in raw)")
        continue
    }
    $secText = $orig.Substring($span.Start, $span.End - $span.Start)
    $decoded = $Cp949.GetString($Latin1.GetBytes($secText))
    # NOTE: check hangul in VALUES, not anywhere in the section - shipping files
    # contain Korean comments next to already-STR-converted values.
    $valuePattern = [regex]'(?m)^(?<pre>[^=\r\n;#\[/][^=\r\n]*=[ \t]*)(?<val>[^\r\n]*[가-힣][^\r\n]*)(?=\r?$)'
    if ($valuePattern.Matches($decoded).Count -eq 0) {
        $classB.Add("$($d.Rel) [$($d.Section)] (raw section has no hangul values - already STR in shipping)")
        continue
    }
    $classA.Add($d)
}

Write-Host ""
Write-Host "=== $Mode report ==="
Write-Host "class A (restorable from raw):"
foreach ($a in ($classA | Sort-Object Rel, Section)) { Write-Host "  RESTORE $($a.Rel) [$($a.Section)]" }
Write-Host "class B (no raw source anywhere - left as-is, matches shipping):"
foreach ($b in ($classB | Sort-Object -Unique)) { Write-Host "  SKIP $b" }

if ($Mode -eq 'Fix' -and $classA.Count -gt 0) {
    # group by file
    $byFile = $classA | Group-Object Rel

    $deletedEntries = 0
    foreach ($g in $byFile) {
        $rel = $g.Name
        $curPath = Join-Path $ConfigDir ($rel -replace '/', '\')
        $origPath = Join-Path $OrigConfig ($rel -replace '/', '\')
        $cur = Read-Latin1 -FilePath $curPath
        $orig = Read-Latin1 -FilePath $origPath

        # splice each dangling section with the raw version (from last to first
        # so earlier indexes stay valid)
        $spans = [System.Collections.Generic.List[object]]::new()
        foreach ($d in $g.Group) {
            $span = Find-Section -Text $orig -Section $d.Section
            if ($null -eq $span) { continue }
            $rawSec = $orig.Substring($span.Start, $span.End - $span.Start)
            $spans.Add(@{ Section = $d.Section; RawSec = $rawSec })
        }
        # apply in reverse document order of the CURRENT file
        $curSpans = [System.Collections.Generic.List[object]]::new()
        foreach ($sp in $spans) {
            $cs = Find-Section -Text $cur -Section $sp.Section
            if ($null -eq $cs) { Write-Host "  WARN section $($sp.Section) missing in current $rel"; continue }
            $curSpans.Add(@{ Start = $cs.Start; End = $cs.End; RawSec = $sp.RawSec; Section = $sp.Section })
        }
        $sorted = $curSpans | Sort-Object { $_.Start } -Descending
        $sb = [System.Text.StringBuilder]::new()
        $cursor = $cur.Length
        foreach ($sp in $sorted) {
            [void]$sb.Insert(0, $cur.Substring($sp.End, $cursor - $sp.End))
            [void]$sb.Insert(0, $sp.RawSec)
            $cursor = $sp.Start
        }
        [void]$sb.Insert(0, $cur.Substring(0, $cursor))
        Write-Latin1 -FilePath $curPath -Text $sb.ToString()
        Write-Host "RESTORED $rel : $($spans.Count) sections"
    }

    # delete the dangling sections' entries from the table
    $kept = [System.Collections.Generic.List[string]]::new()
    $deleted = 0
    foreach ($line in ($tableText -split "`r`n")) {
        $drop = $false
        foreach ($d in $classA) {
            $pat = '^\|INI_' + [regex]::Escape($d.Base) + '::' + [regex]::Escape($d.Section) + '_\d+\|'
            if ($line -match $pat) { $drop = $true; break }
        }
        if ($drop) { $deleted++ } else { [void]$kept.Add($line) }
    }
    $outText = ($kept -join "`r`n")
    Write-Latin1 -FilePath $TextTable -Text $outText
    Write-Host "deleted $deleted table entries"

    Write-Host ""
    Write-Host "next step: pwsh scripts/ini-str-convert.ps1 -Mode Convert -IncludeFile <affected files>"
}
