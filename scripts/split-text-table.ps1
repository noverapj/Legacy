#!/usr/bin/env pwsh
<#
.SYNOPSIS
Splits the flat string tables (text.txt / text_en.txt) into the per-locale
app/ui/config layout used by ioStringManager.

.DESCRIPTION
    text.txt    -> kr/app.txt (exe_), kr/ui.txt (xml_), kr/config.txt (ini_)
    text_en.txt -> us/app.txt (exe_), us/ui.txt, us/config.txt (only if non-empty)

    Lines are processed byte-preserving: the file is decoded as Latin-1
    (ISO-8859-1, a lossless byte<->char bijection) so CP949 content is
    never re-encoded. Output files are byte-identical to their input lines.

    Non-conforming lines (not |key|text|) are reported and skipped.
    Existing target files are not overwritten unless -Force is given.

.PARAMETER TextDir
    Directory that contains text.txt (and optionally text_en.txt).

.EXAMPLE
    pwsh scripts/split-text-table.ps1 -TextDir data/client/resource/text
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$TextDir,

    [switch]$Force
)

$ErrorActionPreference = 'Stop'

$Latin1 = [System.Text.Encoding]::GetEncoding(28591)
$LinePattern = '^\|[^|\r\n]+\|[^|\r\n]*\|?$'

function Read-AllLinesLatin1 {
    param([string]$Path)
    $bytes = [System.IO.File]::ReadAllBytes($Path)
    $text = $Latin1.GetString($bytes)
    # Split on any newline (CRLF, LF, CR). Safe for CP949: 0x0A/0x0D can
    # never appear as a DBCS trail byte, so a lone 0x0A/0x0D is always a
    # real line ending. text.txt uses LF for the header + exe_ section and
    # CRLF for the rest; text_en.txt is LF-only throughout.
    return ,$text -split '\r\n|\n|\r'
}

function Split-Table {
    param(
        [string]$SourcePath,
        [string]$TargetLocaleDir
    )

    if (-not (Test-Path -LiteralPath $SourcePath)) {
        Write-Host "SKIP (not found): $SourcePath"
        return
    }

    Write-Host "Splitting $SourcePath -> $TargetLocaleDir"
    $lines = Read-AllLinesLatin1 -Path $SourcePath

    $buckets = @{ app = [System.Collections.Generic.List[string]]::new()
                  ui = [System.Collections.Generic.List[string]]::new()
                  config = [System.Collections.Generic.List[string]]::new() }
    $skipped = [System.Collections.Generic.List[string]]::new()
    $headers = [System.Collections.Generic.List[string]]::new()
    $matched = 0

    foreach ($line in $lines) {
        if ($line -eq '') { continue }
        if ($line -match '^\[v\d+\]$') {
            $headers.Add($line)
            continue
        }
        # A handful of legacy lines carry trailing whitespace after the
        # closing pipe; the engine parser ignores it. Trim and keep the
        # clean entry.
        $clean = $line.TrimEnd()
        if ($clean -notmatch $LinePattern) {
            $skipped.Add($line)
            continue
        }
        if ($clean.StartsWith('|exe_', [System.StringComparison]::OrdinalIgnoreCase)) {
            $buckets['app'].Add($clean)
        }
        elseif ($clean.StartsWith('|xml_', [System.StringComparison]::OrdinalIgnoreCase)) {
            $buckets['ui'].Add($clean)
        }
        elseif ($clean.StartsWith('|ini_', [System.StringComparison]::OrdinalIgnoreCase)) {
            $buckets['config'].Add($clean)
        }
        else {
            $skipped.Add($line)
            continue
        }
        $matched++
    }

    if (Test-Path -LiteralPath $TargetLocaleDir) {
        if (-not $Force) {
            throw "Target dir exists (use -Force): $TargetLocaleDir"
        }
    }
    else {
        New-Item -ItemType Directory -Path $TargetLocaleDir -Force | Out-Null
    }

    foreach ($bucket in @('app', 'ui', 'config')) {
        $list = $buckets[$bucket]
        if ($list.Count -eq 0) {
            Write-Host "  $bucket.txt  : 0 entries (not created)"
            continue
        }
        $outPath = Join-Path $TargetLocaleDir "$bucket.txt"
        if ((Test-Path -LiteralPath $outPath) -and -not $Force) {
            throw "Target file exists (use -Force): $outPath"
        }
        $content = ($list -join "`r`n") + "`r`n"
        [System.IO.File]::WriteAllBytes($outPath, $Latin1.GetBytes($content))
        Write-Host ("  {0,-10}: {1,7} entries -> {2}" -f "$bucket.txt", $list.Count, $outPath)
    }

    Write-Host ("  matched   : {0,7}   headers: {1}   skipped: {2}" -f $matched, $headers.Count, $skipped.Count)
    foreach ($header in $headers) {
        Write-Host "  version header (dropped): $header"
    }
    foreach ($bad in $skipped) {
        $preview = if ($bad.Length -gt 80) { $bad.Substring(0, 80) + '...' } else { $bad }
        Write-Host "  SKIPPED: $preview"
    }
    Write-Host ''
}

if (-not (Test-Path -LiteralPath $TextDir)) {
    throw "TextDir not found: $TextDir"
}

Split-Table -SourcePath (Join-Path $TextDir 'text.txt') -TargetLocaleDir (Join-Path $TextDir 'kr')
Split-Table -SourcePath (Join-Path $TextDir 'text_en.txt') -TargetLocaleDir (Join-Path $TextDir 'us')

Write-Host 'Done. Old flat files are left in place; remove them after runtime verification.'
