[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$Server,

    [Parameter(Mandatory = $true)]
    [string]$Database,

    [Parameter(Mandatory = $true)]
    [string]$User,

    [Parameter(Mandatory = $true)]
    [string]$Password,

    [Parameter(Mandatory = $true)]
    [string]$Output,

    [ValidateSet("ANSI", "ASCII", "UTF8", "UTF8BOM", "UNICODE", "UTF32")]
    [string]$Encoding = "UNICODE",

    [ValidateSet("Windows", "Linux")]
    [string]$Target = "Windows"
)

$ErrorActionPreference = "Stop"

# ============================================================
# LOAD SMO
# ============================================================

try {
    Import-Module SqlServer -ErrorAction Stop
}
catch {
    Write-Host ""
    Write-Host "ERROR: Module 'SqlServer' not found." -ForegroundColor Red
    Write-Host ""
    Write-Host "Install it with:" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Install-Module SqlServer -Scope CurrentUser" -ForegroundColor Cyan
    Write-Host ""
    exit 1
}

# ============================================================
# OUTPUT PATH
# ============================================================

$outputPath = [System.IO.Path]::GetFullPath(
    [System.IO.Path]::Combine(
        (Get-Location).Path,
        $Output
    )
)

$outputDirectory = Split-Path -Parent $outputPath

if (-not (Test-Path $outputDirectory)) {
    New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
}

# ============================================================
# ENCODING
# ============================================================

switch ($Encoding.ToUpper()) {

    "ANSI" {
        # CP949 (Korean Windows ANSI code page), so Korean
        # comments inside stored procedures survive intact.
        # .NET Core / 5+ requires the code pages provider.
        [System.Text.Encoding]::RegisterProvider(
            [System.Text.CodePagesEncodingProvider]::Instance
        )
        $textEncoding = [System.Text.Encoding]::GetEncoding(949)
    }

    "ASCII" {
        $textEncoding = [System.Text.Encoding]::ASCII
    }

    "UTF8" {
        $textEncoding = New-Object System.Text.UTF8Encoding($false)
    }

    "UTF8BOM" {
        $textEncoding = New-Object System.Text.UTF8Encoding($true)
    }

    "UNICODE" {
        # UTF-16 LE + BOM
        # Closest match to SSMS Unicode output.
        $textEncoding = [System.Text.Encoding]::Unicode
    }

    "UTF32" {
        $textEncoding = [System.Text.Encoding]::UTF32
    }
}

# ============================================================
# TARGET PLATFORM
#
# The data/log file paths (FILENAME = N'...') emitted by SMO
# point to the source server. Rewrite them to the default
# data directory of the target platform.
# ============================================================

switch ($Target.ToLower()) {

    "windows" {
        $targetDataPath = "C:\Program Files\Microsoft SQL Server\MSSQL\DATA\"
    }

    "linux" {
        $targetDataPath = "/var/opt/mssql/data/"
    }
}

# ============================================================
# DEPENDENCY ORDERING
#
# CREATE VIEW resolves referenced objects immediately (no
# deferred name resolution), so a view referencing another
# view must never be emitted before it. CREATE PROCEDURE only
# warns, but the warnings are noisy. This function orders
# same-kind objects so referenced objects come first.
#
# Matching is token-based and exact ("userFriendListDB" must
# NOT match a reference to "userFriendListDB_table"), and
# case-insensitive like SQL. Objects stuck in a dependency
# cycle fall back to their original (alphabetical) order.
# ============================================================

function Order-ObjectsByDependencies {

    param(
        [object[]]$Objects,
        [scriptblock]$GetText
    )

    $count = $Objects.Count

    if ($count -lt 2) {
        return $Objects
    }

    $tokensOf = New-Object 'object[]' $count

    for ($i = 0; $i -lt $count; $i++) {

        $text = & $GetText $Objects[$i]

        $tokens = [System.Collections.Generic.HashSet[string]]::new(
            [System.StringComparer]::OrdinalIgnoreCase
        )

        foreach ($match in [regex]::Matches($text, '\w+')) {
            $null = $tokens.Add($match.Value)
        }

        $tokensOf[$i] = $tokens
    }

    $depsOf = New-Object 'object[]' $count

    for ($i = 0; $i -lt $count; $i++) {

        $deps = [System.Collections.Generic.HashSet[int]]::new()

        for ($j = 0; $j -lt $count; $j++) {

            if ($i -ne $j -and $tokensOf[$i].Contains($Objects[$j].Name)) {
                $null = $deps.Add($j)
            }
        }

        $depsOf[$i] = $deps
    }

    $ordered = New-Object System.Collections.Generic.List[object]

    $remaining = [System.Collections.Generic.HashSet[int]]::new()
    for ($i = 0; $i -lt $count; $i++) {
        $null = $remaining.Add($i)
    }

    while ($remaining.Count -gt 0) {

        $progress = $false

        foreach ($i in @($remaining)) {

            $blocked = $false

            foreach ($j in $depsOf[$i]) {
                if ($remaining.Contains($j)) {
                    $blocked = $true
                    break
                }
            }

            if (-not $blocked) {
                $ordered.Add($Objects[$i])
                $null = $remaining.Remove($i)
                $progress = $true
            }
        }

        if (-not $progress) {
            foreach ($i in ($remaining | Sort-Object)) {
                $ordered.Add($Objects[$i])
            }
            break
        }
    }

    return $ordered
}

# ============================================================
# CONNECTION
# ============================================================

Write-Host ""
Write-Host "============================================================" -ForegroundColor DarkGray
Write-Host " SQL Server Database Script Generator" -ForegroundColor Cyan
Write-Host "============================================================" -ForegroundColor DarkGray
Write-Host ""

Write-Host "Server   : $Server"
Write-Host "Database : $Database"
Write-Host "Output   : $outputPath"
Write-Host "Encoding : $Encoding"
Write-Host "Target   : $Target ($targetDataPath)"
Write-Host ""

$serverConnection = New-Object `
    Microsoft.SqlServer.Management.Common.ServerConnection

$serverConnection.ServerInstance = $Server
$serverConnection.LoginSecure = $false
$serverConnection.Login = $User
$serverConnection.Password = $Password

# ============================================================
# CONNECT
# ============================================================

Write-Host "Connecting..." -ForegroundColor Cyan

$serverConnection.Connect()

if (-not $serverConnection.IsOpen) {
    throw "Failed to open connection to SQL Server."
}

$srv = New-Object `
    Microsoft.SqlServer.Management.Smo.Server(
        $serverConnection
    )

Write-Host "Connected." -ForegroundColor Green

# ============================================================
# DATABASE
# ============================================================

$db = $srv.Databases[$Database]

if ($null -eq $db) {
    throw "Database '$Database' not found."
}

Write-Host "Database found." -ForegroundColor Green
Write-Host ""

# ============================================================
# BULK-LOAD DEFINITION TEXT
#
# View/StoredProcedure/UserDefinedFunction bodies are lazily
# loaded (TextBody is empty right after enumeration). Ask SMO
# to fetch TextBody with the initial enumeration so the
# dependency ordering can read every body in one round trip.
# ============================================================

$srv.SetDefaultInitFields(
    [Microsoft.SqlServer.Management.Smo.View],
    "TextBody"
)

$srv.SetDefaultInitFields(
    [Microsoft.SqlServer.Management.Smo.StoredProcedure],
    "TextBody"
)

$srv.SetDefaultInitFields(
    [Microsoft.SqlServer.Management.Smo.UserDefinedFunction],
    "TextBody"
)

# ============================================================
# SCRIPTING OPTIONS
# ============================================================

$options = New-Object `
    Microsoft.SqlServer.Management.Smo.ScriptingOptions

# ------------------------------------------------------------
# Database
#
# IncludeDatabaseContext = $false: the script writes
# "USE [LosaGame]" once (see DATABASE CONTEXT section).
# With $true, SMO re-emits USE [..] + GO before every object,
# which bloats the output massively.
# ------------------------------------------------------------

$options.ScriptSchema = $true
$options.ScriptData = $false

$options.IncludeDatabaseContext = $false

$options.IncludeHeaders = $true

$options.IncludeIfNotExists = $false

# ------------------------------------------------------------
# Dependencies
#
# WithDependencies = $false: objects are each scripted exactly
# once. With $true, every procedure that references a table
# re-emits that table's CREATE statement (e.g. userMemberDB
# appeared 210x), inflating the output ~4x. The loops below
# already emit objects in a valid creation order.
# ------------------------------------------------------------

$options.WithDependencies = $false

# ------------------------------------------------------------
# Tables
# ------------------------------------------------------------

$options.Indexes = $true

$options.ClusteredIndexes = $true

$options.NonClusteredIndexes = $true

# ------------------------------------------------------------
# Constraints
# ------------------------------------------------------------

$options.DriAll = $true
$options.DriAllConstraints = $true
$options.DriAllKeys = $true
$options.DriChecks = $true
$options.DriDefaults = $true
$options.DriForeignKeys = $true
$options.DriPrimaryKey = $true
$options.DriUniqueKeys = $true

# ------------------------------------------------------------
# Triggers
# ------------------------------------------------------------

$options.Triggers = $true

# ------------------------------------------------------------
# Security
# ------------------------------------------------------------

$options.Permissions = $true
$options.ScriptOwner = $true

$options.IncludeDatabaseRoleMemberships = $true

$options.LoginSid = $true

# ------------------------------------------------------------
# Extended properties
# ------------------------------------------------------------

$options.ExtendedProperties = $true

# ------------------------------------------------------------
# Full text
# ------------------------------------------------------------

$options.FullTextCatalogs = $true
$options.FullTextIndexes = $true
$options.FullTextStopLists = $true

# ------------------------------------------------------------
# Partitioning
# ------------------------------------------------------------

$options.NoTablePartitioningSchemes = $false
$options.NoIndexPartitioningSchemes = $false

# ------------------------------------------------------------
# XML / spatial
# ------------------------------------------------------------

$options.XmlIndexes = $true
$options.SpatialIndexes = $true

# ------------------------------------------------------------
# Compression
# ------------------------------------------------------------

$options.ScriptDataCompression = $true
$options.ScriptXmlCompression = $true

# ------------------------------------------------------------
# Change tracking
# ------------------------------------------------------------

$options.ChangeTracking = $true

# ------------------------------------------------------------
# Batch terminator
# ------------------------------------------------------------

$options.ScriptBatchTerminator = $true

$options.BatchSize = 1

# ------------------------------------------------------------
# Target SQL Server version
#
# Use current server version so generated syntax follows
# the actual SQL Server.
# ------------------------------------------------------------

try {
    $versionMap = @{
        8  = "Version80"
        9  = "Version90"
        10 = "Version100"
        11 = "Version110"
        12 = "Version120"
        13 = "Version130"
        14 = "Version140"
        15 = "Version150"
        16 = "Version160"
        17 = "Version170"
    }

    $versionName = $versionMap[[int]$srv.Version.Major]

    if ($versionName) {
        $options.TargetServerVersion = `
            [Microsoft.SqlServer.Management.Smo.SqlServerVersion]::$versionName
    }
    else {
        Write-Host `
            "Warning: Unknown server version $($srv.Version)." `
            -ForegroundColor Yellow
    }
}
catch {
    Write-Host "Warning: Could not set TargetServerVersion automatically." `
        -ForegroundColor Yellow
}

# ============================================================
# OPEN OUTPUT
# ============================================================

$writer = New-Object System.IO.StreamWriter(
    $outputPath,
    $false,
    $textEncoding
)

try {

    # ========================================================
    # HEADER
    #
    # Everything here must be a valid SQL comment: bare text
    # lines get parsed as SQL ("Script generated by ..." used
    # to fail with "Incorrect syntax near the keyword 'by'").
    # ========================================================

    $writer.WriteLine(
        "/****** Object:  Database [$Database] ******/"
    )

    $writer.WriteLine("/*")

    $writer.WriteLine(
        "Script generated by PowerShell + SQL Server SMO"
    )

    $writer.WriteLine(
        "Server: $Server"
    )

    $writer.WriteLine(
        "Generated: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
    )

    $writer.WriteLine("*/")

    $writer.WriteLine("")

    # ========================================================
    # MASTER
    # ========================================================

    $writer.WriteLine("USE [master]")
    $writer.WriteLine("GO")
    $writer.WriteLine("")

    # ========================================================
    # DATABASE CREATE
    #
    # Database.Script() is the important part here.
    # This is different from Transfer.ScriptTransfer().
    # ========================================================

    Write-Host "Generating CREATE DATABASE..." -ForegroundColor Cyan

    # WithDependencies is not supported when scripting the
    # Database object itself: SMO dependency discovery only
    # accepts object types (Table, StoredProcedure, ...).
    # It is already $false globally (see Dependencies above).

    $databaseScript = $db.Script($options)

    # Database.Script() emits the database users (CREATE USER +
    # GRANT CONNECT, in the right order) itself. Track them so
    # the USERS loop below does not emit the same user twice.
    $scriptedUsers = [System.Collections.Generic.HashSet[string]]::new()

    foreach ($statement in $databaseScript) {

        if ([string]::IsNullOrWhiteSpace($statement)) {
            continue
        }

        # Note: with IncludeHeaders = $true the statement string
        # starts with the "/****** Object ... ******/" header,
        # so the pattern must not be anchored with ^.
        if ($statement -match 'CREATE USER \[([^\]]+)\]') {
            $null = $scriptedUsers.Add($Matches[1])
        }

        # Rewrite the data/log file locations (FILENAME = N'...')
        # to the target platform's default data directory, keeping
        # the original file name (e.g. LosaGame.mdf / LosaGame_log.ldf).
        $statement = [regex]::Replace(
            $statement,
            "FILENAME\s*=\s*N'([^']+)'",
            {
                param($match)

                $fileName = [System.IO.Path]::GetFileName(
                    $match.Groups[1].Value
                )

                return "FILENAME = N'$targetDataPath$fileName'"
            }
        )

        $writer.WriteLine($statement.TrimEnd())

        if (-not $statement.TrimEnd().EndsWith("GO")) {
            $writer.WriteLine("GO")
        }

        $writer.WriteLine("")
    }

    # ========================================================
    # EXPLICIT DATABASE SETTINGS
    #
    # Some SSMS database settings are emitted separately
    # depending on SMO / SQL Server version.
    # ========================================================

    Write-Host "Generating database settings..." -ForegroundColor Cyan

    $db.Refresh()

    # --------------------------------------------------------
    # Compatibility level
    #
    # Database.CompatibilityVersion is deprecated and returns
    # $null in SMO 22.x; use Database.CompatibilityLevel.
    # The enum's numeric value equals the compatibility level
    # (e.g. Version160 -> 160).
    # --------------------------------------------------------

    if ($null -ne $db.CompatibilityLevel) {

        $compatibilityValue = [int]$db.CompatibilityLevel

        if ($compatibilityValue -gt 0) {

            $writer.WriteLine(
                "ALTER DATABASE [$Database] SET COMPATIBILITY_LEVEL = $compatibilityValue"
            )

            $writer.WriteLine("GO")
            $writer.WriteLine("")
        }
    }

    # ========================================================
    # DATABASE CONTEXT
    # ========================================================

    $writer.WriteLine("USE [$Database]")
    $writer.WriteLine("GO")
    $writer.WriteLine("")

    # ========================================================
    # SCHEMAS
    # ========================================================

    Write-Host "Generating schemas..." -ForegroundColor Cyan

    # Dependency discovery does not support the Schema object
    # type. WithDependencies is already $false globally.

    foreach ($schema in $db.Schemas) {

        if ($schema.IsSystemObject) {
            continue
        }

        try {
            $script = $schema.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {
            Write-Host "Warning: Schema [$($schema.Name)] failed to script." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # TABLES
    # ========================================================

    Write-Host "Generating tables..." -ForegroundColor Cyan

    foreach ($table in $db.Tables) {

        if ($table.IsSystemObject) {
            continue
        }

        Write-Host "  Table: [$($table.Schema)].[$($table.Name)]"

        try {

            $script = $table.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Table [$($table.Schema)].[$($table.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # VIEWS
    # ========================================================

    Write-Host "Generating views..." -ForegroundColor Cyan

    # CREATE VIEW validates references immediately: order the
    # views so referenced views are emitted first.
    $orderedViews = Order-ObjectsByDependencies `
        -Objects @($db.Views | Where-Object { -not $_.IsSystemObject }) `
        -GetText { param($o) $o.TextBody }

    foreach ($view in $orderedViews) {

        Write-Host "  View: [$($view.Schema)].[$($view.Name)]"

        try {

            $script = $view.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: View [$($view.Schema)].[$($view.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # STORED PROCEDURES
    # ========================================================

    Write-Host "Generating stored procedures..." -ForegroundColor Cyan

    # Order procedures by their references to each other so
    # SQL Server does not emit "depends on the missing object"
    # warnings at CREATE time.
    $orderedProcedures = Order-ObjectsByDependencies `
        -Objects @($db.StoredProcedures | Where-Object { -not $_.IsSystemObject }) `
        -GetText { param($o) $o.TextBody }

    foreach ($procedure in $orderedProcedures) {

        Write-Host `
            "  Procedure: [$($procedure.Schema)].[$($procedure.Name)]"

        try {

            $script = $procedure.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Procedure [$($procedure.Schema)].[$($procedure.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # USER DEFINED FUNCTIONS
    # ========================================================

    Write-Host "Generating functions..." -ForegroundColor Cyan

    $orderedFunctions = Order-ObjectsByDependencies `
        -Objects @($db.UserDefinedFunctions | Where-Object { -not $_.IsSystemObject }) `
        -GetText { param($o) $o.TextBody }

    foreach ($function in $orderedFunctions) {

        Write-Host `
            "  Function: [$($function.Schema)].[$($function.Name)]"

        try {

            $script = $function.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Function [$($function.Schema)].[$($function.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # SEQUENCES
    # ========================================================

    Write-Host "Generating sequences..." -ForegroundColor Cyan

    foreach ($sequence in $db.Sequences) {

        Write-Host `
            "  Sequence: [$($sequence.Schema)].[$($sequence.Name)]"

        try {

            $script = $sequence.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Sequence [$($sequence.Schema)].[$($sequence.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # SYNONYMS
    # ========================================================

    Write-Host "Generating synonyms..." -ForegroundColor Cyan

    foreach ($synonym in $db.Synonyms) {

        Write-Host `
            "  Synonym: [$($synonym.Schema)].[$($synonym.Name)]"

        try {

            $script = $synonym.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Synonym [$($synonym.Schema)].[$($synonym.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # DATABASE ROLES
    # ========================================================

    Write-Host "Generating roles..." -ForegroundColor Cyan

    # Dependency discovery does not support the DatabaseRole
    # object type. WithDependencies is already $false globally.

    foreach ($role in $db.Roles) {

        if ($role.IsFixedRole) {
            continue
        }

        try {

            $script = $role.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Role [$($role.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # USERS
    # ========================================================

    Write-Host "Generating users..." -ForegroundColor Cyan

    # Dependency discovery does not support the User object
    # type. WithDependencies is already $false globally.

    foreach ($userObject in $db.Users) {

        if ($userObject.IsSystemObject) {
            continue
        }

        # Already scripted by Database.Script() in the CREATE
        # DATABASE section (CREATE USER + GRANT CONNECT).
        # A second CREATE USER would fail on re-run.
        if ($scriptedUsers.Contains($userObject.Name)) {
            Write-Host `
                "  User [$($userObject.Name)] already scripted with the database, skipping." `
                -ForegroundColor DarkGray
            continue
        }

        try {

            $script = $userObject.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: User [$($userObject.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # DATABASE TRIGGERS
    # ========================================================

    Write-Host "Generating database triggers..." -ForegroundColor Cyan

    foreach ($trigger in $db.Triggers) {

        try {

            $script = $trigger.Script($options)

            foreach ($statement in $script) {

                if ([string]::IsNullOrWhiteSpace($statement)) {
                    continue
                }

                $writer.WriteLine($statement.TrimEnd())
                $writer.WriteLine("GO")
                $writer.WriteLine("")
            }
        }
        catch {

            Write-Host `
                "Warning: Database trigger [$($trigger.Name)] failed." `
                -ForegroundColor Yellow
        }
    }

    # ========================================================
    # EXTENDED PROPERTIES
    # ========================================================

    Write-Host "Generating extended properties..." -ForegroundColor Cyan

    try {

        $script = $db.Script($options)

        # Extended properties that are returned by object scripting
        # are already included through ScriptingOptions.ExtendedProperties.
    }
    catch {
        # Ignore duplicate database script invocation.
    }

    # ========================================================
    # END
    # ========================================================

    $writer.Flush()
}
finally {

    $writer.Dispose()
}

# ============================================================
# DISCONNECT
# ============================================================

$serverConnection.Disconnect()

# ============================================================
# RESULT
# ============================================================

$fileInfo = Get-Item $outputPath

Write-Host ""
Write-Host "============================================================" `
    -ForegroundColor DarkGray

Write-Host " DONE" -ForegroundColor Green

Write-Host "============================================================" `
    -ForegroundColor DarkGray

Write-Host ""

Write-Host "Output   : $outputPath"
Write-Host "Size     : $([math]::Round($fileInfo.Length / 1MB, 2)) MB"
Write-Host "Encoding : $Encoding"

Write-Host ""
