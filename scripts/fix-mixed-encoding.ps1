#Requires -Version 7.0

[CmdletBinding()]
param(
    [string]$Path = '',
    [string]$IncludeFile = ''
)

$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $Path) { $Path = Join-Path $RepoRoot 'data/client/config' }

$CP = [System.Text.Encoding]::GetEncoding(949,
    [System.Text.EncoderFallback]::ExceptionFallback,
    [System.Text.DecoderFallback]::ExceptionFallback)
$UTF8 = [System.Text.Encoding]::GetEncoding(65001,
    [System.Text.EncoderFallback]::ExceptionFallback,
    [System.Text.DecoderFallback]::ExceptionFallback)
$Hang = [regex]'[가-힣]'

$files = Get-ChildItem -LiteralPath $Path -Recurse -File -Filter '*.ini'
if ($IncludeFile) {
    $wanted = $IncludeFile.Split(',') | ForEach-Object { $_.Trim().ToLowerInvariant() } | Where-Object { $_ }
    $files = $files | Where-Object { $wanted -contains $_.Name.ToLowerInvariant() }
}

$fixedFiles = 0
$unfixable = 0
$clean = 0

foreach ($f in $files) {
    $b = [System.IO.File]::ReadAllBytes($f.FullName)
    $bomStripped = $false
    if ($b.Length -ge 3 -and $b[0] -eq 0xEF -and $b[1] -eq 0xBB -and $b[2] -eq 0xBF) {
        $b2 = [byte[]]::new($b.Length - 3)
        [array]::Copy($b, 3, $b2, 0, $b2.Length)
        $b = $b2
        $bomStripped = $true
    }
    try {
        $null = $CP.GetString($b)
        if ($bomStripped) {
            [System.IO.File]::WriteAllBytes($f.FullName, $b)
            $fixedFiles++
            Write-Host ("FIXED {0}  bom-stripped" -f $f.FullName.Substring($Path.Length + 1))
        } else { $clean++ }
        continue
    } catch {}

    $out = [System.Collections.Generic.List[byte]]::new()
    $i = 0
    $lineFixes = 0
    $lineBad = 0
    $inCharTotal = 0
    while ($i -lt $b.Length) {
        $nl = [array]::IndexOf($b, [byte]10, $i)
        if ($nl -lt 0) { $nl = $b.Length - 1; $lineEnd = $b.Length } else { $lineEnd = $nl; if ($lineEnd -gt $i -and $b[$lineEnd-1] -eq 13) { $lineEnd-- } }

        $lineBytes = [byte[]]::new($lineEnd - $i)
        [array]::Copy($b, $i, $lineBytes, 0, $lineBytes.Length)

        $handled = $false
        try {
            $lineText = $CP.GetString($lineBytes)
            $inCharTotal += $lineText.Length
            foreach ($x in $lineBytes) { $out.Add($x) }
            $handled = $true
        } catch {}

        if (-not $handled) {
            try {
                $txt = $UTF8.GetString($lineBytes)
                $inCharTotal += $txt.Length
                $newBytes = $CP.GetBytes($txt)
                foreach ($x in $newBytes) { $out.Add($x) }
                $handled = $true
                $lineFixes++
            } catch {}
        }

        if (-not $handled) {
            foreach ($x in $lineBytes) { $out.Add($x) }
            $lineBad++
        }

        if ($lineEnd -lt $b.Length -and $b[$lineEnd] -eq 13) { $out.Add(13) }
        if ($nl -lt $b.Length - 1) { $out.Add(10) }
        elseif ($nl -eq $b.Length - 1) { $out.Add(10) }
        $i = $nl + 1
    }

    $result = $out.ToArray()
    $strictOk = $false
    $decoded = $null
    try { $decoded = $CP.GetString($result); $strictOk = $true } catch {}

    $outCharTotal = 0
    if ($strictOk) { $outCharTotal = ([regex]::Matches($decoded, '[^\r\n]')).Count }

    if ($strictOk -and $lineBad -eq 0 -and $outCharTotal -eq $inCharTotal) {
        [System.IO.File]::WriteAllBytes($f.FullName, $result)
        $fixedFiles++
        Write-Host ("FIXED {0}  lines={1} chars={2}/{3}" -f $f.FullName.Substring($Path.Length + 1), $lineFixes, $outCharTotal, $inCharTotal)
    }
    else {
        $unfixable++
        Write-Host ("ABORTED {0}  strict={1} badlines={2} chars={3}/{4} (file left unchanged)" -f $f.FullName, $strictOk, $lineBad, $outCharTotal, $inCharTotal)
    }
}

Write-Host "=== DONE: already-clean=$clean fixed=$fixedFiles unfixable=$unfixable total=$($files.Count) ==="
