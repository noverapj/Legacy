#Requires -Version 7.0

[CmdletBinding()]
param(
    [ValidateSet('Scan','Convert','Verify')]
    [string]$Mode = 'Scan',
    [string]$XmlDir = '',
    [string]$TextTable = '',
    [string[]]$IncludeFile = @()
)

$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $XmlDir) { $XmlDir = Join-Path $RepoRoot 'data/client/xml' }
if (-not $TextTable) { $TextTable = Join-Path $RepoRoot 'data/client/resource/text/kr/ui.txt' }

$Enc = [System.Text.Encoding]::GetEncoding(949,
    [System.Text.EncoderFallback]::ExceptionFallback,
    [System.Text.DecoderFallback]::ExceptionFallback)

$Hangeul = [regex]'[가-힣]'
$StrRefPattern = [regex]'STR\((\d+)\)'
$AttrPattern = [regex]'([\w-]+)\s*=\s*"([^"]*)"'
$TextEntryPattern = [regex]'^\|([^|]+)\|([^|]*)\|$'

function Read-StrictCp949 {
    param([string]$FilePath)
    try {
        return $Enc.GetString([System.IO.File]::ReadAllBytes($FilePath))
    } catch {
        return $null
    }
}

function Find-Spans {
    param([string]$Text)

    $spans = [System.Collections.Generic.List[object]]::new()
    $len = $Text.Length
    $i = 0
    $stack = [System.Collections.Generic.List[string]]::new()

    while ($i -lt $len) {
        $lt = $Text.IndexOf('<', $i)
        if ($lt -lt 0) { break }
        if ($lt + 3 -lt $len -and $Text.Substring($lt, 4) -eq '<!--') {
            $end = $Text.IndexOf('-->', $lt + 4)
            if ($end -lt 0) { break }
            $i = $end + 3
            continue
        }
        $c2 = if ($lt + 1 -lt $len) { $Text[$lt + 1] } else { '' }
        if ($c2 -eq '?') {
            $end = $Text.IndexOf('?>', $lt)
            if ($end -lt 0) { break }
            $i = $end + 2
            continue
        }
        if ($c2 -eq '/') {
            $gt = $Text.IndexOf('>', $lt)
            if ($gt -lt 0) { break }
            $closeName = $Text.Substring($lt + 2, $gt - $lt - 2).Trim()
            if ($stack.Count -gt 0 -and $stack[$stack.Count - 1] -eq $closeName) {
                $stack.RemoveAt($stack.Count - 1)
            }
            $i = $gt + 1
            continue
        }

        $j = $lt + 1
        $inQuote = $false
        while ($j -lt $len) {
            $ch = $Text[$j]
            if ($inQuote) {
                if ($ch -eq '"') { $inQuote = $false }
            }
            elseif ($ch -eq '"') { $inQuote = $true }
            elseif ($ch -eq '>') { break }
            $j++
        }
        if ($j -ge $len) { break }

        $selfClosing = ($Text[$j - 1] -eq '/')
        $header = $Text.Substring($lt, $j - $lt + 1)
        $nameMatch = [regex]::Match($header, '^<[A-Za-z_][\w.-]*')
        if (-not $nameMatch.Success) { $i = $j + 1; continue }
        $name = $nameMatch.Value.Substring(1)
        $parent = if ($stack.Count -gt 0) { $stack[$stack.Count - 1] } else { '' }

        if ($name -eq 'ExtraInfo' -and $parent -eq 'Window') {
            foreach ($m in $AttrPattern.Matches($header)) {
                $val = [System.Net.WebUtility]::HtmlDecode($m.Groups[2].Value)
                if ($Hangeul.IsMatch($val)) {
                    $spans.Add(@{ Kind='attr'; Start=($lt + $m.Groups[2].Index); End=($lt + $m.Groups[2].Index + $m.Groups[2].Length); Value=$val; Route='ExtraInfo' })
                }
            }
        }
        elseif ($name -eq 'TextBlock') {
            foreach ($m in $AttrPattern.Matches($header)) {
                if ($m.Groups[1].Value -eq 'Text') {
                    $val = [System.Net.WebUtility]::HtmlDecode($m.Groups[2].Value)
                    if ($Hangeul.IsMatch($val)) {
                        $spans.Add(@{ Kind='attr'; Start=($lt + $m.Groups[2].Index); End=($lt + $m.Groups[2].Index + $m.Groups[2].Length); Value=$val; Route='TextBlock' })
                    }
                }
            }
        }
        elseif ($name -eq 'Help' -and $parent -eq 'Tooltip') {
            foreach ($m in $AttrPattern.Matches($header)) {
                if ($m.Groups[1].Value -eq 'Text') {
                    $val = [System.Net.WebUtility]::HtmlDecode($m.Groups[2].Value)
                    if ($Hangeul.IsMatch($val)) {
                        $spans.Add(@{ Kind='attr'; Start=($lt + $m.Groups[2].Index); End=($lt + $m.Groups[2].Index + $m.Groups[2].Length); Value=$val; Route='Help' })
                    }
                }
            }
        }
        elseif ($name -eq 'Text' -and ($parent -eq 'Title' -or $parent -eq 'Text')) {
            if (-not $selfClosing) {
                $close = $Text.IndexOf('</Text>', $j + 1)
                if ($close -ge 0) {
                    $contentStart = $j + 1
                    $raw = $Text.Substring($contentStart, $close - $contentStart)
                    if (-not $raw.Contains('<')) {
                        $val = [System.Net.WebUtility]::HtmlDecode($raw)
                        if ($Hangeul.IsMatch($val)) {
                            $spans.Add(@{ Kind='text'; Start=$contentStart; End=$close; Value=$val; Route=$parent })
                        }
                    }
                }
            }
        }

        if (-not $selfClosing) { $stack.Add($name) }
        $i = $j + 1
    }

    return ,$spans
}

function Test-Convertible {
    param([string]$Value)
    if ($Value.Contains('|')) { return 'pipe-in-text' }
    if ($Value -match 'STR\(') { return 'str-in-text' }
    if ($Value -match '[\r\n]') { return 'newline-in-text' }
    if ($Value -match '\\[rn]') { return 'escape-in-text' }
    return $null
}

function Load-TextKeys {
    param([string]$TablePath)
    $keys = [System.Collections.Generic.HashSet[string]]::new()
    $content = Read-StrictCp949 -FilePath $TablePath
    if ($null -eq $content) { throw "Cannot decode text table as CP949: $TablePath" }
    foreach ($line in ($content -split "`r?`n")) {
        $m = $TextEntryPattern.Match($line)
        if ($m.Success) { [void]$keys.Add($m.Groups[1].Value.ToLowerInvariant()) }
    }
    return ,$keys
}

$files = Get-ChildItem $XmlDir -Recurse -Filter '*.xml' | Sort-Object FullName
$files = $files | Where-Object { $_.DirectoryName -notmatch '\\xml\\xml$' }
if ($IncludeFile.Count -gt 0) {
    $wanted = $IncludeFile | ForEach-Object { $_.Split(',') } | ForEach-Object { $_.Trim() } | Where-Object { $_ } | ForEach-Object { $_.ToLowerInvariant() }
    $files = $files | Where-Object { $wanted -contains $_.Name.ToLowerInvariant() }
}

if ($Mode -eq 'Verify') {
    $keys = Load-TextKeys -TablePath $TextTable
    $missing = [System.Collections.Generic.List[string]]::new()
    $referenced = [System.Collections.Generic.HashSet[string]]::new()
    $badFiles = 0
    foreach ($f in $files) {
        $s = Read-StrictCp949 -FilePath $f.FullName
        if ($null -eq $s) { Write-Host "DECODE-FAIL: $($f.Name)"; $badFiles++; continue }
        foreach ($m in $StrRefPattern.Matches($s)) {
            $key = "xml_$($f.BaseName.ToLowerInvariant())_$($m.Groups[1].Value)".ToLowerInvariant()
            [void]$referenced.Add($key)
            if (-not $keys.Contains($key)) {
                $missing.Add("$($f.Name): STR($($m.Groups[1].Value)) -> $key")
            }
        }
    }
    $orphans = $keys | Where-Object { $_ -like 'xml_*' -and -not $referenced.Contains($_) }

    Write-Host "=== VERIFY: $($files.Count) files ==="
    Write-Host "Missing text table entries : $($missing.Count)"
    $missing | Select-Object -First 30 | ForEach-Object { Write-Host "  MISSING $_" }
    Write-Host "Orphan xml_* entries       : $(@($orphans).Count)"
    @($orphans) | Select-Object -First 10 | ForEach-Object { Write-Host "  ORPHAN $_" }
    if ($badFiles -gt 0) { Write-Host "Undecodable files: $badFiles" }
    exit 0
}

$textKeys = Load-TextKeys -TablePath $TextTable
$appendEntries = [System.Collections.Generic.List[string]]::new()
$totalSpans = 0
$totalSkipped = 0
$totalFilesTouched = 0
$decodeFails = 0
$keyBaseCounts = @{}

foreach ($f in $files) {
    $s = Read-StrictCp949 -FilePath $f.FullName
    if ($null -eq $s) {
        Write-Host "DECODE-FAIL: $($f.Name) (skipped)"
        $decodeFails++
        continue
    }

    $base = $f.BaseName.ToLowerInvariant()
    if ($keyBaseCounts.ContainsKey($base)) {
        Write-Host "WARN duplicate base name: $($f.FullName) shares key base '$base'"
    }
    else { $keyBaseCounts[$base] = 1 }

    $spans = Find-Spans -Text $s
    $convertible = [System.Collections.Generic.List[object]]::new()
    $skipped = 0
    foreach ($sp in $spans) {
        $reason = Test-Convertible -Value $sp.Value
        if ($reason) {
            $skipped++
            Write-Host "  SKIP $($f.Name) [$($sp.Route)] ($reason): $($sp.Value.Substring(0, [Math]::Min(30, $sp.Value.Length)))"
        }
        else {
            $convertible.Add($sp)
        }
    }

    if ($Mode -eq 'Scan') {
        if ($convertible.Count -gt 0 -or $skipped -gt 0) {
            $byRoute = $convertible | Group-Object Route | ForEach-Object { "$($_.Name):$($_.Count)" }
            Write-Host ("SCAN {0,-40} convertible={1,-4} skipped={2,-3} [{3}]" -f $f.Name, $convertible.Count, $skipped, ($byRoute -join ' '))
        }
        $totalSpans += $convertible.Count
        $totalSkipped += $skipped
        continue
    }

    if ($convertible.Count -eq 0) { continue }

    $maxExisting = 0
    foreach ($m in $StrRefPattern.Matches($s)) {
        $n = [int]$m.Groups[1].Value
        if ($n -gt $maxExisting) { $maxExisting = $n }
    }

    $out = [System.Text.StringBuilder]::new()
    $cursor = 0
    $next = $maxExisting
    foreach ($sp in $convertible) {
        [void]$out.Append($s.Substring($cursor, $sp.Start - $cursor))
        do { $next++ } while ($textKeys.Contains("xml_${base}_$next".ToLowerInvariant()))
        [void]$out.Append("STR($next)")
        $entryKey = "XML_$($f.BaseName)_$next"
        $appendEntries.Add("|$entryKey|$($sp.Value)|")
        $cursor = $sp.End
        [void]$textKeys.Add($entryKey.ToLowerInvariant())
    }
    [void]$out.Append($s.Substring($cursor))

    $result = $out.ToString()
    $hangeulBefore = $Hangeul.Matches($s).Count
    $hangeulConverted = 0
    foreach ($sp in $convertible) { $hangeulConverted += $Hangeul.Matches($sp.Value).Count }
    $hangeulAfter = $Hangeul.Matches($result).Count
    $strCountAfter = $StrRefPattern.Matches($result).Count
    $strCountBefore = $StrRefPattern.Matches($s).Count

    if ($strCountAfter -ne ($strCountBefore + $convertible.Count) -or
        $hangeulAfter -ne ($hangeulBefore - $hangeulConverted)) {
        Write-Host "SELFCHECK-FAIL: $($f.Name) - aborted"
        continue
    }

    [System.IO.File]::WriteAllBytes($f.FullName, $Enc.GetBytes($result))
    $totalFilesTouched++
    $totalSpans += $convertible.Count
    $totalSkipped += $skipped
    Write-Host ("CONVERTED {0,-40} strings={1,-4} next={2}" -f $f.Name, $convertible.Count, $next)
}

if ($Mode -eq 'Scan') {
    Write-Host "=== SCAN TOTAL: convertible=$totalSpans skipped=$totalSkipped decodeFails=$decodeFails across $($files.Count) files ==="
}
else {
    if ($appendEntries.Count -gt 0) {
        $tableBytes = [System.IO.File]::ReadAllBytes($TextTable)
        $needsCrlf = ($tableBytes.Length -lt 2) -or -not (($tableBytes[$tableBytes.Length - 2] -eq 13) -and ($tableBytes[$tableBytes.Length - 1] -eq 10))
        if ($tableBytes.Length -gt 0 -and $needsCrlf) { [System.IO.File]::AppendAllText($TextTable, "`r`n", $Enc) }
        foreach ($e in $appendEntries) {
            [System.IO.File]::AppendAllText($TextTable, "$e`r`n", $Enc)
        }
    }
    Write-Host "=== CONVERT DONE: files=$totalFilesTouched strings=$totalSpans skipped=$totalSkipped newEntries=$($appendEntries.Count) decodeFails=$decodeFails ==="
}
