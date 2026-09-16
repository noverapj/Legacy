#Requires -Version 7.0

[CmdletBinding()]
param(
    [ValidateSet('Scan','Convert','Verify')]
    [string]$Mode = 'Scan',
    [string]$ConfigDir = '',
    [string]$TextTable = '',
    [string[]]$IncludeFile = @()
)

$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $ConfigDir) { $ConfigDir = Join-Path $RepoRoot 'data/client/config' }
if (-not $TextTable) { $TextTable = Join-Path $RepoRoot 'data/client/resource/text/kr/config.txt' }

$Enc = [System.Text.Encoding]::GetEncoding(949,
    [System.Text.EncoderFallback]::ExceptionFallback,
    [System.Text.DecoderFallback]::ExceptionFallback)

$SectionPattern = [regex]'(?m)^\[(?<sec>[^\]\r\n]+)\]'
$ValuePattern = [regex]'(?m)^(?<pre>[^=\r\n;#\[/][^=\r\n]*=[ \t]*)(?<val>[^\r\n]*[가-힣][^\r\n]*)(?=\r?$)'
$StrPattern = [regex]'STR\((?<n>\d+)\)'
$TextEntryPattern = [regex]'^\|([^|]+)\|([^|]*)\|$'

$allTargets = [System.Collections.Generic.List[string]]::new()
foreach ($f in (Get-ChildItem $ConfigDir -Recurse -File -Filter "*.ini" | Sort-Object FullName)) {
    $rel = $f.FullName.Substring($ConfigDir.Length + 1)
    $allTargets.Add($rel)
}

function Read-StrictCp949 {
    param([string]$FilePath)
    try {
        return $Enc.GetString([System.IO.File]::ReadAllBytes($FilePath))
    } catch {
        return $null
    }
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

function Resolve-Sections {
    param([object]$Matches, [int[]]$Positions, [string[]]$Names)
    $sections = [string[]]::new($Matches.Count)
    $si = -1
    for ($i = 0; $i -lt $Matches.Count; $i++) {
        $pos = $Matches[$i].Index
        while (($si + 1) -lt $Positions.Count -and $Positions[$si + 1] -lt $pos) { $si++ }
        if ($si -ge 0) { $sections[$i] = $Names[$si] } else { $sections[$i] = '' }
    }
    return ,$sections
}

$textKeys = Load-TextKeys -TablePath $TextTable
$appendEntries = [System.Collections.Generic.List[string]]::new()
$totalSpans = 0
$totalSkipped = 0
$totalDeduped = 0
$filesTouched = 0
$decodeFails = 0
$script:verifyMissing = 0

$targets = $allTargets
if ($IncludeFile.Count -gt 0) {
    $wanted = $IncludeFile | ForEach-Object { $_.Split(',') } | ForEach-Object { $_.Trim() } | Where-Object { $_ } | ForEach-Object { [System.IO.Path]::GetFileName($_).ToLowerInvariant() }
    $targets = $allTargets | Where-Object { $wanted -contains [System.IO.Path]::GetFileName($_).ToLowerInvariant() }
}

$verifyFolder = $null
$verifyFolderIdx = 0
$verifyFolderTotal = 0
$verifySw = [System.Diagnostics.Stopwatch]::StartNew()

if ($Mode -eq 'Verify') {
    $ordered = [System.Collections.Generic.List[string]]::new()
    foreach ($t in ($targets | Where-Object { $_ -notmatch '[\\/]' })) { $ordered.Add($t) }
    $groups = $targets | Where-Object { $_ -match '[\\/]' } | Group-Object { ($_ -split '[\\/]')[0] } | Sort-Object Name
    foreach ($g in $groups) {
        foreach ($t in $g.Group) { $ordered.Add($t) }
    }
    $targets = $ordered
    Write-Host "=== VERIFY START: table load + $($targets.Count) files, folder-by-folder ==="
}

foreach ($rel in $targets) {
    if ($Mode -eq 'Verify') {
        $folder = if ($rel -match '[\\/]') { ($rel -split '[\\/]')[0] } else { '(root)' }
        if ($folder -ne $verifyFolder) {
            if ($null -ne $verifyFolder) {
                Write-Host ("  done {0}: {1} files, {2:N1}s" -f $verifyFolder, $verifyFolderIdx, $verifySw.Elapsed.TotalSeconds)
            }
            $verifyFolder = $folder
            $verifyFolderIdx = 0
            $verifySw.Restart()
            $verifyFolderTotal = @($targets | Where-Object { ($(if ($_ -match '[\\/]') { ($_ -split '[\\/]')[0] } else { '(root)' })) -eq $folder }).Count
            Write-Host "VERIFY $folder ($verifyFolderTotal files)"
        }
        $verifyFolderIdx++
        if ($verifyFolderIdx % 200 -eq 0) {
            Write-Host ("  progress {0}/{1} ({2:N0}s)" -f $verifyFolderIdx, $verifyFolderTotal, $verifySw.Elapsed.TotalSeconds)
        }
    }

    $full = Join-Path $ConfigDir ($rel -replace '^config/', '')
    if (-not (Test-Path -LiteralPath $full)) { continue }
    $s = Read-StrictCp949 -FilePath $full
    if ($null -eq $s) {
        Write-Host "DECODE-FAIL: $rel"
        $decodeFails++
        continue
    }

    $base = [System.IO.Path]::GetFileNameWithoutExtension($rel).ToLowerInvariant()

    $secMatches = $SectionPattern.Matches($s)
    $secPos = [int[]]::new($secMatches.Count)
    $secName = [string[]]::new($secMatches.Count)
    for ($i = 0; $i -lt $secMatches.Count; $i++) {
        $secPos[$i] = $secMatches[$i].Index
        $secName[$i] = $secMatches[$i].Groups['sec'].Value.Trim()
    }

    $strMatches = $StrPattern.Matches($s)
    if ($strMatches.Count -gt 0) {
        $strSections = Resolve-Sections -Matches $strMatches -Positions $secPos -Names $secName
    } else {
        $strSections = [string[]]::new(0)
    }

    $sectionMax = @{}
    for ($i = 0; $i -lt $strMatches.Count; $i++) {
        $sec = $strSections[$i]
        $n = [int]$strMatches[$i].Groups['n'].Value
        if (-not $sectionMax.ContainsKey($sec) -or $n -gt $sectionMax[$sec]) { $sectionMax[$sec] = $n }
    }

    $valMatches = $ValuePattern.Matches($s)
    if ($Mode -eq 'Verify') {
        $referenced = [System.Collections.Generic.HashSet[string]]::new()
        for ($i = 0; $i -lt $strMatches.Count; $i++) {
            $key = "ini_${base}::$($strSections[$i])_$($strMatches[$i].Groups['n'].Value)".ToLowerInvariant()
            [void]$referenced.Add($key)
            if (-not $textKeys.Contains($key)) {
                $script:verifyMissing++
                Write-Host "MISSING $rel [$($strSections[$i])] $($strMatches[$i].Value) -> $key"
            }
        }
        if ($null -ne $script:verifyRefs) {
            foreach ($k in $referenced) { [void]$script:verifyRefs.Add($k) }
        } else {
            $script:verifyRefs = $referenced
        }
        continue
    }

    $valSections = Resolve-Sections -Matches $valMatches -Positions $secPos -Names $secName

    $candidates = [System.Collections.Generic.List[object]]::new()
    $skipped = 0
    for ($i = 0; $i -lt $valMatches.Count; $i++) {
        $m = $valMatches[$i]
        $val = $m.Groups['val'].Value.TrimEnd()
        $reason = $null
        if ($val.Contains('|')) { $reason = 'pipe-in-value' }
        elseif ($val -match 'STR\(') { $reason = 'str-in-value' }
        elseif ($val -match '\\[rn]') { $reason = 'escape-in-value' }
        elseif ($val -match '\.(txt|ini|xml)\s*$') { $reason = 'file-ref' }
        elseif ($m.Groups['pre'].Value -match 'object_item\d+_name\s*=\s*$') { $reason = 'item-ref' }
        if ($reason) {
            $skipped++
            Write-Host "  SKIP $rel [$($valSections[$i])] ($reason): $($val.Substring(0, [Math]::Min(30, $val.Length)))"
            continue
        }
        $spanStart = $m.Index + $m.Groups['pre'].Length
        $candidates.Add(@{ Section = $valSections[$i]; Start = $spanStart; End = ($m.Index + $m.Length); Value = $val })
    }

    if ($Mode -eq 'Scan') {
        if ($candidates.Count -gt 0 -or $skipped -gt 0) {
            Write-Host ("SCAN {0,-44} convertible={1,-6} skipped={2,-3}" -f (Split-Path $rel -Leaf), $candidates.Count, $skipped)
        }
        $totalSpans += $candidates.Count
        $totalSkipped += $skipped
        continue
    }

    if ($candidates.Count -eq 0) { continue }

    $out = [System.Text.StringBuilder]::new()
    $cursor = 0
    $nextNum = @{}
    $dedupe = @{}
    $dedupedCount = 0
    foreach ($sp in $candidates) {
        [void]$out.Append($s.Substring($cursor, $sp.Start - $cursor))
        $sec = $sp.Section
        if (-not $nextNum.ContainsKey($sec)) {
            $seed = if ($sectionMax.ContainsKey($sec)) { $sectionMax[$sec] } else { 0 }
            $nextNum[$sec] = $seed
            $dedupe[$sec] = @{}
        }
        if ($dedupe[$sec].ContainsKey($sp.Value)) {
            $n = $dedupe[$sec][$sp.Value]
            $dedupedCount++
        }
        else {
            $n = ++$nextNum[$sec]
            $probe = "ini_${base}::${sec}_$n".ToLowerInvariant()
            while ($textKeys.Contains($probe)) {
                $n = ++$nextNum[$sec]
                $probe = "ini_${base}::${sec}_$n".ToLowerInvariant()
            }
            $dedupe[$sec][$sp.Value] = $n
            $appendEntries.Add("|INI_$base::$($sec)_$n|$($sp.Value)|")
            [void]$textKeys.Add($probe)
        }
        [void]$out.Append("STR($n)")
        $cursor = $sp.End
    }
    [void]$out.Append($s.Substring($cursor))

    $result = $out.ToString()
    $hangeulBefore = 0
    $hangeulConverted = 0
    $hang = [regex]::new('[가-힣]')
    $hangeulBefore = $hang.Matches($s).Count
    foreach ($sp in $candidates) {
        $hangeulConverted += $hang.Matches($s.Substring($sp.Start, $sp.End - $sp.Start)).Count
    }
    $hangeulAfter = $hang.Matches($result).Count
    $linesBefore = ([regex]::Matches($s, "`n")).Count
    $linesAfter = ([regex]::Matches($result, "`n")).Count

    if ($hangeulAfter -ne ($hangeulBefore - $hangeulConverted) -or $linesBefore -ne $linesAfter) {
        Write-Host "SELFCHECK-FAIL: $rel - aborted"
        continue
    }

    [System.IO.File]::WriteAllBytes($full, $Enc.GetBytes($result))
    $filesTouched++
    $totalSpans += $candidates.Count
    $totalSkipped += $skipped
    $totalDeduped += $dedupedCount
    Write-Host ("CONVERTED {0,-44} strings={1,-6} deduped={2,-5} skipped={3}" -f (Split-Path $rel -Leaf), $candidates.Count, $dedupedCount, $skipped)
}

if ($Mode -eq 'Verify') {
    if ($null -ne $verifyFolder) {
        Write-Host ("  done {0}: {1} files, {2:N1}s" -f $verifyFolder, $verifyFolderIdx, $verifySw.Elapsed.TotalSeconds)
    }
    $orphans = @($textKeys | Where-Object { $_ -like 'ini_*' -and -not $script:verifyRefs.Contains($_) })
    Write-Host "=== VERIFY DONE: missing=$($script:verifyMissing) orphans=$($orphans.Count) ==="
    $orphans | Select-Object -First 10 | ForEach-Object { Write-Host "  ORPHAN $_" }
    exit 0
}

if ($Mode -eq 'Scan') {
    Write-Host "=== SCAN TOTAL: convertible=$totalSpans skipped=$totalSkipped decodeFails=$decodeFails ==="
}
else {
    if ($appendEntries.Count -gt 0) {
        $tableBytes = [System.IO.File]::ReadAllBytes($TextTable)
        $needsCrlf = ($tableBytes.Length -lt 2) -or -not (($tableBytes[$tableBytes.Length - 2] -eq 13) -and ($tableBytes[$tableBytes.Length - 1] -eq 10))
        if ($tableBytes.Length -gt 0 -and $needsCrlf) { [System.IO.File]::AppendAllText($TextTable, "`r`n", $Enc) }
        $bulk = [string]::Join("`r`n", $appendEntries) + "`r`n"
        [System.IO.File]::AppendAllText($TextTable, $bulk, $Enc)
    }
    Write-Host "=== CONVERT DONE: files=$filesTouched strings=$totalSpans deduped=$totalDeduped skipped=$totalSkipped newEntries=$($appendEntries.Count) decodeFails=$decodeFails ==="
}
