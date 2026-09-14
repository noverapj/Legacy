#region

using System.Data.SqlClient;
using System.Data.SQLite;
using System.Diagnostics;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using ClassLibrary;

#endregion

namespace ApplicationForm;

/// <summary>
///     Debug wrapper for SqlServerToSqLite to diagnose issues with table selection dialog
/// </summary>
public class SqlConversionDebugWrapper
{
    // Reference to the main form
    private static Form _mainForm;

    // Reference to main form's LogMessage method (found via reflection)
    private static MethodInfo _logMessageMethod;

    // Connection string to source database - saved for validation
    private static string _sourceConnString;

    // List of selected tables - saved for validation
    private static List<TableSchema> _selectedTables;

    // Source table row counts - saved to compare with target
    private static readonly Dictionary<string, int> _sourceRowCounts = new();

    /// <summary>
    ///     Wraps the SqlServerToSqLite.ConvertSqlServerToSqLiteDatabase method with additional debugging
    ///     and ensures table selection dialog is shown
    /// </summary>
    public static void ConvertWithDebug(
        string sqlServerConnString,
        string sqlitePath,
        string password,
        SqlConversionHandler handler,
        SqlTableSelectionHandler selectionHandler,
        FailedViewDefinitionHandler viewFailureHandler,
        bool createTriggers,
        bool createViews,
        bool treatGuidAsString,
        bool silent,
        bool debug = false,
        bool timestampAsInt64 = false)
    {
        Console.WriteLine("[DebugWrapper] Starting SQL Server to SQLite conversion");

        try
        {
            _sourceConnString = sqlServerConnString; // Save for later use

            // Step 1: Force disable silent mode
            SetSilentFieldToFalse();

            // Step 2: Get the form reference through the handler if available
            _mainForm = GetMainFormFromHandler(handler);

            LogToMainForm("[Debug] Starting SQL Server to SQLite conversion with detailed logging");

            // Step 3: Fetch the database schema directly
            Console.WriteLine("[DebugWrapper] Fetching database schema...");
            LogToMainForm("[Debug] Fetching database schema from SQL Server...");
            var schema = GetDatabaseSchema(sqlServerConnString);
            if (schema == null || schema.Count == 0)
            {
                Console.WriteLine("[DebugWrapper] No tables found in database");
                LogToMainForm("[Debug] No tables found in database - nothing to convert", 2); // Use error level
                handler?.Invoke(true, false, 0, "No tables found in the database");
                return;
            }

            Console.WriteLine($"[DebugWrapper] Found {schema.Count} tables in database");
            LogToMainForm($"[Debug] Found {schema.Count} tables with detailed column information:");

            // Store row counts for later comparison
            CountSourceTableRows(sqlServerConnString, schema);

            LogSchemaDetails(schema, sqlServerConnString);

            // Step 4: Call the selection handler directly to show the table selection dialog
            List<TableSchema> selectedTables = null;
            if (selectionHandler != null)
            {
                Console.WriteLine("[DebugWrapper] Showing table selection dialog...");
                LogToMainForm("[Debug] Showing table selection dialog...");
                selectedTables = selectionHandler.Invoke(schema);

                // Save for validation later
                _selectedTables = selectedTables;
            }

            // Step 5: Check if tables were selected
            if (selectedTables == null || selectedTables.Count == 0)
            {
                Console.WriteLine("[DebugWrapper] No tables were selected for conversion");
                LogToMainForm("[Debug] No tables were selected for conversion - operation canceled", 2);
                handler?.Invoke(true, false, 0, "No tables were selected for conversion");
                return;
            }

            Console.WriteLine($"[DebugWrapper] {selectedTables.Count} tables selected for conversion");
            LogToMainForm($"[Debug] {selectedTables.Count} tables selected for conversion");

            // Step 6: Create a special table selection handler that returns our pre-selected tables
            // This way we bypass the library's own dialog
            SqlTableSelectionHandler fixedSelectionHandler = delegate
            {
                Console.WriteLine($"[FixedSelectionHandler] Returning {selectedTables.Count} pre-selected tables");
                LogToMainForm($"[Debug] Returning {selectedTables.Count} pre-selected tables to converter");
                return selectedTables;
            };

            // Create a wrapped handler that will run verification AFTER the main conversion is done
            SqlConversionHandler wrappedHandler = (done, success, percent, msg) =>
            {
                Console.WriteLine(
                    $"[ConversionHandler] Called with: done={done}, success={success}, percent={percent}, msg={msg}");

                // If conversion finished, check the resulting SQLite database and transfer any missing data
                if (done && success)
                {
                    // First check if the database file was actually created
                    if (File.Exists(sqlitePath))
                    {
                        LogToMainForm(
                            $"[Debug] SQLite database file created: {sqlitePath} ({new FileInfo(sqlitePath).Length:N0} bytes)");
                        // Now verify and fix any data inconsistencies
                        VerifySqLiteDatabase(sqlitePath, password, selectedTables);
                        AttemptManualDataTransfer(sqlServerConnString, sqlitePath, password, selectedTables);
                    }
                    else
                    {
                        // If the file wasn't created, create an empty SQLite database file
                        LogToMainForm("[Debug] SQLite file not created, creating a new empty database", 2);
                        CreateEmptySqLiteDatabase(sqlitePath, password, selectedTables);
                    }

                    // Create views once tables (and data) exist. Views depend only
                    // on tables, so this runs after both branches above.
                    if (createViews)
                    {
                        try
                        {
                            var views = GetDatabaseViews(sqlServerConnString);
                            LogToMainForm($"[View] Found {views.Count} views in source database");
                            CreateSqLiteViews(sqlitePath, views);
                        }
                        catch (Exception viewEx)
                        {
                            LogToMainForm($"[View] View conversion failed: {viewEx.Message}", 2);
                        }
                    }
                }
                else if (done && !success)
                {
                    LogToMainForm($"[Debug] Conversion reported failure: {msg}", 2);
                }

                // Call the original handler
                handler?.Invoke(done, success, percent, msg);
            };

            FailedViewDefinitionHandler wrappedViewFailureHandler = vs =>
            {
                Console.WriteLine($"[ViewFailureHandler] Called for view: {vs?.ViewName ?? "unknown"}");
                LogToMainForm($"[Debug] View failure handler called for view: {vs?.ViewName ?? "unknown"}", 1);
                return viewFailureHandler?.Invoke(vs);
            };

            // Ensure SQLite file path directory exists
            EnsureSqLiteDirectoryExists(sqlitePath);

            // Delete existing SQLite file to ensure a clean conversion
            TryDeleteExistingSqLiteFile(sqlitePath);

            // Step 7: Start the actual conversion with our fixed selection handler
            Console.WriteLine("[DebugWrapper] Starting SQL Server to SQLite conversion with selected tables...");
            LogToMainForm("[Debug] Starting SQL Server to SQLite conversion with selected tables...");

            SqlServerToSqLite.ConvertSqlServerToSqLiteDatabase(
                sqlServerConnString,
                sqlitePath,
                password,
                wrappedHandler,
                fixedSelectionHandler, // Use our fixed selection handler that returns pre-selected tables
                wrappedViewFailureHandler,
                createTriggers,
                createViews,
                treatGuidAsString,
                false, // Force silent to false
                debug,
                timestampAsInt64);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Exception in conversion: {ex}");
            LogToMainForm($"[Debug] Exception in conversion: {ex.Message}", 2);
            LogToMainForm($"[Debug] Stack trace: {ex.StackTrace}", 2);
            // Report the error to the handler
            handler?.Invoke(true, false, 0, $"Conversion error: {ex.Message}");
            throw; // Rethrow to maintain original behavior
        }
    }

    /// <summary>
    ///     Creates an empty SQLite database with the necessary tables if the conversion failed
    /// </summary>
    private static void CreateEmptySqLiteDatabase(string sqlitePath, string password, List<TableSchema> tables)
    {
        try
        {
            LogToMainForm("[Debug] Creating empty SQLite database file...");

            // Create a new SQLite database file
            SQLiteConnection.CreateFile(sqlitePath);

            // Create connection string
            var connString = $"Data Source={sqlitePath};Version=3;";
            if (!string.IsNullOrEmpty(password)) connString += $"Password={password};";

            using var connection = new SQLiteConnection(connString);
            connection.Open();
            LogToMainForm("[Debug] Successfully created empty SQLite database");

            // Create tables based on schema
            using (var transaction = connection.BeginTransaction())
            {
                try
                {
                    foreach (var table in tables)
                    {
                        // Build CREATE TABLE statement
                        var createTableSql = new StringBuilder();
                        createTableSql.Append($"CREATE TABLE [{table.TableName}] (");

                        // Add columns
                        for (var i = 0; i < table.Columns.Count; i++)
                        {
                            var column = table.Columns[i];
                            if (i > 0) createTableSql.Append(", ");

                            // Map SQL Server types to SQLite types
                            var sqliteType = MapSqlServerTypeToSqLite(column.ColumnType);

                            // Add column definition
                            createTableSql.Append($"[{column.ColumnName}] {sqliteType}");

                            // Add nullability
                            if (!column.IsNullable)
                                createTableSql.Append(" NOT NULL");

                            // Add primary key for identity columns
                            // (inline PK only valid for single-column PKs; composite PKs
                            // get the table-level clause below)
                            if (column.IsIdentity && table.PrimaryKey.Count == 1 &&
                                table.PrimaryKey[0] == column.ColumnName)
                                createTableSql.Append(" PRIMARY KEY AUTOINCREMENT");
                        }

                        // Add composite primary key if applicable
                        if (table.PrimaryKey.Count > 1 ||
                            (table.PrimaryKey.Count == 1 && !table.Columns.Any(c =>
                                c.ColumnName == table.PrimaryKey[0] && c.IsIdentity)))
                            createTableSql.Append(
                                $", PRIMARY KEY ({string.Join(", ", table.PrimaryKey.Select(pk => $"[{pk}]"))})");

                        createTableSql.Append(")");

                        // Execute CREATE TABLE statement
                        using (var command = new SQLiteCommand(createTableSql.ToString(), connection, transaction))
                        {
                            command.ExecuteNonQuery();
                        }

                        LogToMainForm($"[Debug] Created table structure for '{table.TableName}'");
                    }

                    // Add foreign keys as ALTER TABLE statements after all tables are created
                    foreach (var table in tables)
                    foreach (var fk in table.ForeignKeys)
                    {
                        var alterTableSql = $@"
                                    ALTER TABLE [{table.TableName}] 
                                    ADD CONSTRAINT [FK_{table.TableName}_{fk.ColumnName}] 
                                    FOREIGN KEY ([{fk.ColumnName}]) 
                                    REFERENCES [{fk.ForeignTableName}] ([{fk.ForeignColumnName}])";

                        try
                        {
                            using var command = new SQLiteCommand(alterTableSql, connection, transaction);
                            command.ExecuteNonQuery();
                        }
                        catch (Exception ex)
                        {
                            LogToMainForm($"[Debug] Could not create foreign key {fk.ColumnName}: {ex.Message}", 1);
                        }
                    }

                    transaction.Commit();
                    LogToMainForm("[Debug] Successfully created table structures in SQLite database");
                }
                catch (Exception ex)
                {
                    transaction.Rollback();
                    LogToMainForm($"[Debug] Error creating table structures: {ex.Message}", 2);
                    throw;
                }
            }

            // Now that we've created the empty database, we can attempt to transfer data
            AttemptManualDataTransfer(_sourceConnString, sqlitePath, password, tables);
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Failed to create empty SQLite database: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Fetches all view definitions from the SQL Server database.
    ///     Returns a list of (name, definition) tuples.
    /// </summary>
    private static List<(string Name, string Sql)> GetDatabaseViews(string sqlConnString)
    {
        var views = new List<(string Name, string Sql)>();
        using (var connection = new SqlConnection(sqlConnString))
        {
            connection.Open();
            const string query = @"
SELECT v.name, m.definition
FROM sys.views v
LEFT JOIN sys.sql_modules m ON m.object_id = v.object_id
ORDER BY v.name";
            using var command = new SqlCommand(query, connection);
            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                var name = reader.GetString(0);
                if (reader.IsDBNull(1))
                {
                    LogToMainForm($"[View] Skipping '{name}': no definition available", 1);
                    continue;
                }
                views.Add((name, reader.GetString(1)));
            }
        }

        return views;
    }

    /// <summary>
    ///     Transforms a SQL Server (T-SQL) view definition into SQLite-compatible SQL.
    ///     Handles the common MSSQL constructs found in this codebase:
    ///     - Schema prefixes ([dbo]. / dbo. / DBO.)
    ///     - NOLOCK table hints (WITH (NOLOCK))
    ///     - Unicode literals (N'...')
    ///     - ISNULL(...) -> IFNULL(...)
    /// </summary>
    private static string TransformViewSqlToSqLite(string viewSql)
    {
        var result = viewSql;

        // Remove schema prefixes: "[dbo].[x]" -> "[x]", "[dbo].x" -> "x"
        result = Regex.Replace(result, @"\[dbo\]\.", "", RegexOptions.IgnoreCase);

        // Remove bare schema prefixes: "dbo.x" -> "x" (but not inside identifiers)
        result = Regex.Replace(result, @"\b(dbo|DBO)\.", "");

        // Remove NOLOCK and similar table hints: "WITH (NOLOCK)" / "WITH(NOLOCK)"
        result = Regex.Replace(result, @"\bWITH\s*\(\s*NOLOCK\s*\)", "", RegexOptions.IgnoreCase);

        // Unicode string literals: N'...' -> '...'
        result = Regex.Replace(result, @"\bN'", "'");

        // ISNULL(a, b) -> IFNULL(a, b)
        result = Regex.Replace(result, @"\bISNULL\s*\(", "IFNULL(", RegexOptions.IgnoreCase);

        return result;
    }

    /// <summary>
    ///     Creates the given views in the SQLite database. Uses a multi-pass retry
    ///     loop so inter-view dependencies (a view selecting from another view)
    ///     resolve automatically regardless of alphabetical order. Views that
    ///     still fail after all passes are skipped with a warning.
    /// </summary>
    private static void CreateSqLiteViews(string sqlitePath, List<(string Name, string Sql)> views)
    {
        if (views == null || views.Count == 0)
        {
            LogToMainForm("[View] No views to create");
            return;
        }

        var connString = $"Data Source={sqlitePath};Version=3;";
        using var connection = new SQLiteConnection(connString);
        connection.Open();

        var pending = new List<(string Name, string Sql)>(views);
        var lastErrors = new Dictionary<string, string>();
        const int maxPasses = 10;

        for (var pass = 1; pending.Count > 0 && pass <= maxPasses; pass++)
        {
            var stillPending = new List<(string Name, string Sql)>();

            foreach (var (name, rawSql) in pending)
            {
                var sqliteSql = TransformViewSqlToSqLite(rawSql);
                try
                {
                    using var command = new SQLiteCommand(sqliteSql, connection);
                    command.ExecuteNonQuery();
                    LogToMainForm($"[View] Created '{name}' (pass {pass})");
                    lastErrors.Remove(name);
                }
                catch (Exception ex)
                {
                    // Remember the failure reason and retry on the next pass
                    stillPending.Add((name, rawSql));
                    lastErrors[name] = ex.Message;
                    LogToMainForm($"[View] Pass {pass}: '{name}' failed: {ex.Message}", 1);
                }
            }

            // No progress means remaining failures are not dependency-order issues
            if (stillPending.Count == pending.Count)
            {
                LogToMainForm($"[View] No progress on {stillPending.Count} view(s) - giving up on them", 1);
                break;
            }

            pending = stillPending;
        }

        foreach (var kv in lastErrors)
            LogToMainForm($"[View] SKIPPED '{kv.Key}' (final): {kv.Value}", 2);

        LogToMainForm($"[View] Done: {views.Count - lastErrors.Count}/{views.Count} views created");
    }

    /// <summary>
    ///     Maps SQL Server data types to SQLite data types
    /// </summary>
    private static string MapSqlServerTypeToSqLite(string sqlServerType)
    {
        switch (sqlServerType.ToLower())
        {
            case "bit":
                return "INTEGER";
            case "tinyint":
            case "smallint":
            case "int":
            case "bigint":
            case "numeric":
            case "decimal":
                return "INTEGER";
            case "real":
            case "float":
                return "REAL";
            case "money":
            case "smallmoney":
                return "REAL";
            case "datetime":
            case "smalldatetime":
            case "date":
            case "time":
            case "datetime2":
                return "TEXT";
            case "ntext":
            case "text":
            case "xml":
                return "TEXT";
            case "nchar":
            case "char":
            case "nvarchar":
            case "varchar":
                return "TEXT";
            case "uniqueidentifier":
                return "TEXT";
            case "varbinary":
            case "binary":
                return "BLOB";
            case "sql_variant":
                return "TEXT";
            default:
                return "TEXT";
        }
    }

    /// <summary>
    ///     Stores the row counts from SQL Server tables for later comparison
    /// </summary>
    private static void CountSourceTableRows(string connectionString, List<TableSchema> tables)
    {
        _sourceRowCounts.Clear();

        try
        {
            using var connection = new SqlConnection(connectionString);
            connection.Open();

            foreach (var table in tables)
                try
                {
                    using var command = new SqlCommand($"SELECT COUNT(*) FROM [{table.TableName}]", connection);
                    var rowCount = Convert.ToInt32(command.ExecuteScalar());
                    _sourceRowCounts[table.TableName] = rowCount;
                }
                catch (Exception ex)
                {
                    Console.WriteLine(
                        $"[DebugWrapper] Error counting rows for table {table.TableName}: {ex.Message}");
                }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error counting source rows: {ex.Message}");
        }
    }

    /// <summary>
    ///     Attempts a manual data transfer for tables with row count mismatches
    /// </summary>
    private static void AttemptManualDataTransfer(string sourceConnString, string sqlitePath, string password,
        List<TableSchema> tables)
    {
        try
        {
            LogToMainForm("[Debug] Checking for tables with missing data...");

            if (!File.Exists(sqlitePath))
            {
                LogToMainForm("[Debug] SQLite database file does not exist, cannot transfer data", 2);
                return;
            }

            var anyEmpty = false;
            foreach (var table in tables)
                if (_sourceRowCounts.TryGetValue(table.TableName, out var sourceRowCount) && sourceRowCount > 0)
                {
                    try
                    {
                        // Get the SQLite row count
                        var sqliteRowCount = GetSqLiteTableRowCount(sqlitePath, password, table.TableName);

                        // If SQLite has fewer rows than source, attempt to transfer data directly
                        if (sqliteRowCount >= sourceRowCount) continue;
                        anyEmpty = true;
                        LogToMainForm(
                            $"[Debug] Table '{table.TableName}' has {sourceRowCount} rows in SQL Server but only {sqliteRowCount} in SQLite. Attempting manual transfer...",
                            1);
                        ManuallyTransferTableData(sourceConnString, sqlitePath, password, table);
                    }
                    catch (Exception ex)
                    {
                        LogToMainForm($"[Debug] Error checking row count for table '{table.TableName}': {ex.Message}",
                            2);
                        // Try to transfer data anyway
                        anyEmpty = true;
                        ManuallyTransferTableData(sourceConnString, sqlitePath, password, table);
                    }
                }

            // If we had to manually transfer data, verify the database again
            if (!anyEmpty) return;
            LogToMainForm("[Debug] Verifying database after manual data transfer...");
            VerifySqLiteDatabase(sqlitePath, password, tables);
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Error during manual data transfer: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Attempts to manually transfer data from SQL Server to SQLite for a specific table
    /// </summary>
    private static void ManuallyTransferTableData(string sourceConnString, string sqlitePath, string password,
        TableSchema table)
    {
        try
        {
            // Build column list for SQL statements
            var columnList = string.Join(", ", table.Columns.Select(c => $"[{c.ColumnName}]"));

            // Get data from source
            var rows = new List<Dictionary<string, object>>();
            using (var connection = new SqlConnection(sourceConnString))
            {
                connection.Open();
                try
                {
                    using var command = new SqlCommand($"SELECT {columnList} FROM [{table.TableName}]", connection);
                    using var reader = command.ExecuteReader();
                    while (reader.Read())
                    {
                        var row = new Dictionary<string, object>();
                        for (var i = 0; i < reader.FieldCount; i++)
                        {
                            var columnName = reader.GetName(i);
                            var value = reader.IsDBNull(i) ? DBNull.Value : reader.GetValue(i);
                            row[columnName] = value;
                        }

                        rows.Add(row);
                    }
                }
                catch (Exception ex)
                {
                    LogToMainForm($"[Debug] Error reading data from SQL Server table '{table.TableName}': {ex.Message}",
                        2);
                    return;
                }
            }

            LogToMainForm($"[Debug] Retrieved {rows.Count} rows from '{table.TableName}' in SQL Server");

            if (rows.Count == 0)
            {
                LogToMainForm($"[Debug] No data to transfer for table '{table.TableName}'");
                return;
            }

            // First try clearing the target table
            var connString = $"Data Source={sqlitePath};Version=3;" +
                             (string.IsNullOrEmpty(password) ? "" : $"Password={password};");
            using (var connection = new SQLiteConnection(connString))
            {
                try
                {
                    connection.Open();
                }
                catch (Exception ex)
                {
                    LogToMainForm($"[Debug] Could not open SQLite database: {ex.Message}", 2);
                    return;
                }

                // Use a transaction for efficiency and to ensure atomicity
                using var transaction = connection.BeginTransaction();
                try
                {
                    // Verify that the table exists
                    using var checkTableCommand = new SQLiteCommand(
                        "SELECT name FROM sqlite_master WHERE type='table' AND name=@TableName", connection,
                        transaction);
                    checkTableCommand.Parameters.AddWithValue("@TableName", table.TableName);
                    var tableName = checkTableCommand.ExecuteScalar();

                    if (tableName == null)
                    {
                        LogToMainForm(
                            $"[Debug] Table '{table.TableName}' does not exist in SQLite database. Creating...",
                            1);

                        // Create the table
                        var createTableSql = new StringBuilder();
                        createTableSql.Append($"CREATE TABLE [{table.TableName}] (");

                        // Add columns
                        for (var i = 0; i < table.Columns.Count; i++)
                        {
                            var column = table.Columns[i];
                            if (i > 0) createTableSql.Append(", ");

                            // Map SQL Server types to SQLite types
                            var sqliteType = MapSqlServerTypeToSqLite(column.ColumnType);

                            // Add column definition
                            createTableSql.Append($"[{column.ColumnName}] {sqliteType}");

                            // Add nullability
                            if (!column.IsNullable)
                                createTableSql.Append(" NOT NULL");

                            // Add primary key for identity columns
                            // (inline PK only valid for single-column PKs; composite PKs
                            // get the table-level clause below)
                            if (column.IsIdentity && table.PrimaryKey.Count == 1 &&
                                table.PrimaryKey[0] == column.ColumnName)
                                createTableSql.Append(" PRIMARY KEY AUTOINCREMENT");
                        }

                        // Add composite primary key if applicable
                        if (table.PrimaryKey.Count > 1 ||
                            (table.PrimaryKey.Count == 1 && !table.Columns.Any(c =>
                                c.ColumnName == table.PrimaryKey[0] && c.IsIdentity)))
                            createTableSql.Append(
                                $", PRIMARY KEY ({string.Join(", ", table.PrimaryKey.Select(pk => $"[{pk}]"))})");

                        createTableSql.Append(")");

                        using (var createTableCommand =
                               new SQLiteCommand(createTableSql.ToString(), connection, transaction))
                        {
                            createTableCommand.ExecuteNonQuery();
                        }

                        LogToMainForm($"[Debug] Created table '{table.TableName}' in SQLite database");
                    }

                    // Delete existing data
                    using var deleteCommand =
                        new SQLiteCommand($"DELETE FROM [{table.TableName}]", connection, transaction);
                    deleteCommand.ExecuteNonQuery();

                    // Insert data row by row
                    var insertedRows = 0;
                    foreach (var row in rows)
                        try
                        {
                            // Build parameter list for INSERT
                            var paramNames = string.Join(", ", row.Keys.Select(k => $"@{k}"));
                            var sql =
                                $"INSERT INTO [{table.TableName}] ({string.Join(", ", row.Keys.Select(k => $"[{k}]"))}) VALUES ({paramNames})";

                            using var insertCommand = new SQLiteCommand(sql, connection, transaction);
                            // Add parameters
                            foreach (var pair in row)
                                insertCommand.Parameters.AddWithValue($"@{pair.Key}",
                                    pair.Value == DBNull.Value ? null : pair.Value);

                            insertCommand.ExecuteNonQuery();
                            insertedRows++;
                        }
                        catch (Exception ex)
                        {
                            LogToMainForm($"[Debug] Error inserting row into '{table.TableName}': {ex.Message}", 1);
                            // Continue with other rows
                        }

                    // Commit the transaction
                    transaction.Commit();
                    LogToMainForm(
                        $"[Debug] Manually inserted {insertedRows} of {rows.Count} rows into '{table.TableName}'");
                }
                catch (Exception ex)
                {
                    // Rollback on error
                    transaction.Rollback();
                    LogToMainForm($"[Debug] Error during manual data insert for '{table.TableName}': {ex.Message}",
                        2);
                    throw;
                }
            }
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Failed to manually transfer data for table '{table.TableName}': {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Gets row count from an SQLite table
    /// </summary>
    private static int GetSqLiteTableRowCount(string sqlitePath, string password, string tableName)
    {
        var connString = $"Data Source={sqlitePath};Version=3;" +
                         (string.IsNullOrEmpty(password) ? "" : $"Password={password};");
        using var connection = new SQLiteConnection(connString);
        connection.Open();
        using var command = new SQLiteCommand($"SELECT COUNT(*) FROM [{tableName}]", connection);
        return Convert.ToInt32(command.ExecuteScalar());
    }

    /// <summary>
    ///     Tries to delete the existing SQLite file to ensure a clean conversion
    /// </summary>
    private static void TryDeleteExistingSqLiteFile(string sqlitePath)
    {
        try
        {
            if (!File.Exists(sqlitePath)) return;
            File.Delete(sqlitePath);
            LogToMainForm("[Debug] Deleted existing SQLite file to ensure clean conversion");
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Warning: Could not delete existing SQLite file: {ex.Message}", 1);
        }
    }

    /// <summary>
    ///     Ensures the directory for SQLite database file exists
    /// </summary>
    private static void EnsureSqLiteDirectoryExists(string sqlitePath)
    {
        try
        {
            var directory = Path.GetDirectoryName(sqlitePath);
            if (string.IsNullOrEmpty(directory) || Directory.Exists(directory)) return;
            Directory.CreateDirectory(directory);
            LogToMainForm($"[Debug] Created directory for SQLite database: {directory}");
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Failed to create directory for SQLite database: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Logs detailed information about all tables and columns in the schema
    /// </summary>
    private static void LogSchemaDetails(List<TableSchema> schema, string connectionString)
    {
        var sb = new StringBuilder();

        // Log all tables and their columns
        foreach (var table in schema)
        {
            sb.Clear();
            sb.AppendLine($"  [Table: {table.TableSchemaName}.{table.TableName}]");
            sb.AppendLine($"    - Columns: {table.Columns.Count}");

            foreach (var column in table.Columns)
                sb.AppendLine($"      * {column.ColumnName} ({column.ColumnType}" +
                              $"{(column.Length > 0 ? $"({column.Length})" : "")}" +
                              $"{(column.IsNullable ? ", NULL" : ", NOT NULL")}" +
                              $"{(column.IsIdentity ? ", IDENTITY" : "")}" +
                              $"{(!string.IsNullOrEmpty(column.DefaultValue) ? $", Default: {column.DefaultValue}" : "")})");

            if (table.PrimaryKey.Count > 0)
            {
                sb.Append("    - Primary Key: ");
                sb.AppendLine(string.Join(", ", table.PrimaryKey));
            }

            if (table.ForeignKeys.Count > 0)
            {
                sb.AppendLine("    - Foreign Keys:");
                foreach (var fk in table.ForeignKeys)
                    sb.AppendLine($"      * {fk.ColumnName} -> {fk.ForeignTableName}.{fk.ForeignColumnName}" +
                                  $"{(fk.CascadeOnDelete ? " (CASCADE)" : "")}");
            }

            // Try to get row count for this table
            try
            {
                var rowCount = _sourceRowCounts.ContainsKey(table.TableName)
                    ? _sourceRowCounts[table.TableName]
                    : GetTableRowCount(connectionString, table.TableName);
                sb.AppendLine($"    - Row Count: {rowCount}");
            }
            catch (Exception ex)
            {
                sb.AppendLine($"    - Row Count: Error: {ex.Message}");
            }

            LogToMainForm(sb.ToString());
        }
    }

    /// <summary>
    ///     Get the number of rows in a table
    /// </summary>
    private static int GetTableRowCount(string connectionString, string tableName)
    {
        using var connection = new SqlConnection(connectionString);
        connection.Open();
        using (var command = new SqlCommand($"SELECT COUNT(*) FROM [{tableName}]", connection))
        {
            return Convert.ToInt32(command.ExecuteScalar());
        }
    }

    /// <summary>
    ///     Verifies the resulting SQLite database file after conversion
    /// </summary>
    private static void VerifySqLiteDatabase(string sqlitePath, string password, List<TableSchema> expectedTables)
    {
        try
        {
            LogToMainForm("[Debug] Verifying SQLite database file...");

            if (!File.Exists(sqlitePath))
            {
                LogToMainForm($"[Debug] ERROR: SQLite file does not exist at path: {sqlitePath}", 2);
                return;
            }

            LogToMainForm($"[Debug] SQLite file found: {sqlitePath} ({new FileInfo(sqlitePath).Length:N0} bytes)");

            // Create connection string
            var connString = $"Data Source={sqlitePath};Version=3;";
            if (!string.IsNullOrEmpty(password)) connString += $"Password={password};";

            using var connection = new SQLiteConnection(connString);
            connection.Open();
            LogToMainForm("[Debug] Successfully opened SQLite database connection");

            // Get list of tables
            using (var command = new SQLiteCommand(
                       "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name;",
                       connection))
            {
                using (var reader = command.ExecuteReader())
                {
                    var tables = new List<string>();
                    while (reader.Read()) tables.Add(reader["name"].ToString());

                    LogToMainForm(
                        $"[Debug] Found {tables.Count} tables in SQLite database: {string.Join(", ", tables)}");

                    // Check for missing tables
                    foreach (var expectedTable in expectedTables)
                        if (!tables.Contains(expectedTable.TableName))
                            LogToMainForm($"[Debug] WARNING: Expected table '{expectedTable.TableName}' is missing",
                                1);
                }
            }

            // Check each table
            foreach (var table in expectedTables)
                try
                {
                    // Check if the table exists in SQLite
                    using (var checkCommand = new SQLiteCommand(
                               "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=@TableName",
                               connection))
                    {
                        checkCommand.Parameters.AddWithValue("@TableName", table.TableName);
                        var count = Convert.ToInt32(checkCommand.ExecuteScalar());

                        if (count == 0)
                        {
                            LogToMainForm($"[Debug] Table '{table.TableName}' does not exist in SQLite database",
                                1);
                            continue;
                        }
                    }

                    // Get row count
                    using (var command = new SQLiteCommand($"SELECT COUNT(*) FROM [{table.TableName}]", connection))
                    {
                        var rowCount = Convert.ToInt32(command.ExecuteScalar());
                        LogToMainForm($"[Debug] Table '{table.TableName}' has {rowCount} rows");

                        // Get source row count
                        var sourceRowCount = _sourceRowCounts.ContainsKey(table.TableName)
                            ? _sourceRowCounts[table.TableName]
                            : 0;

                        if (sourceRowCount > 0 && rowCount == 0)
                            // Table exists but has no rows when it should
                            LogToMainForm(
                                $"[Debug] WARNING: Table '{table.TableName}' has 0 rows in SQLite but {sourceRowCount} rows in SQL Server",
                                1);
                        else if (sourceRowCount > 0 && rowCount < sourceRowCount)
                            // Has fewer rows than it should
                            LogToMainForm(
                                $"[Debug] WARNING: Table '{table.TableName}' has {rowCount} rows in SQLite but {sourceRowCount} rows in SQL Server",
                                1);

                        // Sample some data from this table (if has rows)
                        if (rowCount > 0) GetSampleTableData(connection, table.TableName);
                    }
                }
                catch (Exception ex)
                {
                    LogToMainForm($"[Debug] Error checking table '{table.TableName}': {ex.Message}", 2);
                }

            // Check SQLite database structure
            LogToMainForm("[Debug] Analyzing SQLite database structure...");
            CheckSqLiteStructure(connection);
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Failed to verify SQLite database: {ex.Message}", 2);
            LogToMainForm($"[Debug] Stack trace: {ex.StackTrace}", 2);
        }
    }

    /// <summary>
    ///     Checks the SQLite database structure for issues
    /// </summary>
    private static void CheckSqLiteStructure(SQLiteConnection connection)
    {
        try
        {
            // Check if database is valid with integrity check
            using (var command = new SQLiteCommand("PRAGMA integrity_check;", connection))
            {
                var result = command.ExecuteScalar()?.ToString();
                LogToMainForm($"[Debug] Database integrity check result: {result}");

                if (result != "ok") LogToMainForm("[Debug] WARNING: SQLite database failed integrity check!", 1);
            }

            // Get SQLite version
            using (var command = new SQLiteCommand("SELECT sqlite_version();", connection))
            {
                var version = command.ExecuteScalar()?.ToString();
                LogToMainForm($"[Debug] SQLite version: {version}");
            }

            // Check page size
            using (var command = new SQLiteCommand("PRAGMA page_size;", connection))
            {
                var pageSize = Convert.ToInt32(command.ExecuteScalar());
                LogToMainForm($"[Debug] Database page size: {pageSize} bytes");
            }

            // Check if journal is enabled
            using (var command = new SQLiteCommand("PRAGMA journal_mode;", connection))
            {
                var journalMode = command.ExecuteScalar()?.ToString();
                LogToMainForm($"[Debug] Database journal mode: {journalMode}");
            }
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Error checking SQLite structure: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Gets sample data from a table to verify content
    /// </summary>
    private static void GetSampleTableData(SQLiteConnection connection, string tableName)
    {
        try
        {
            using (var command = new SQLiteCommand($"SELECT * FROM [{tableName}] LIMIT 1", connection))
            {
                using (var reader = command.ExecuteReader())
                {
                    if (reader.Read())
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine($"[Debug] Sample row from '{tableName}':");

                        for (var i = 0; i < reader.FieldCount; i++)
                        {
                            var colName = reader.GetName(i);
                            var value = reader.IsDBNull(i) ? "NULL" : reader.GetValue(i);
                            sb.AppendLine($"    {colName} = {value}");
                        }

                        LogToMainForm(sb.ToString());
                    }
                }
            }
        }
        catch (Exception ex)
        {
            LogToMainForm($"[Debug] Error reading sample data from '{tableName}': {ex.Message}", 1);
        }
    }

    /// <summary>
    ///     Logs messages to the main form's log control using reflection if possible
    /// </summary>
    private static void LogToMainForm(string message, int logLevel = 0)
    {
        try
        {
            if (_mainForm != null)
            {
                // Find the LogMessage method if we haven't already
                if (_logMessageMethod == null)
                {
                    var methods = _mainForm.GetType().GetMethods(BindingFlags.Instance | BindingFlags.NonPublic);
                    _logMessageMethod = methods.FirstOrDefault(m => m.Name == "LogMessage" &&
                                                                    m.GetParameters().Length >= 1 &&
                                                                    m.GetParameters()[0].ParameterType ==
                                                                    typeof(string));
                }

                if (_logMessageMethod != null)
                {
                    // Check if it needs a LogLevel parameter
                    var parameters = _logMessageMethod.GetParameters();
                    if (parameters.Length >= 2 && parameters[1].ParameterType.Name.Contains("LogLevel"))
                    {
                        // Get the LogLevel enum type
                        var logLevelType = parameters[1].ParameterType;

                        // Convert our numeric logLevel to the enum value
                        var enumValue = Enum.ToObject(logLevelType, logLevel);

                        // Invoke with the enum parameter
                        _logMessageMethod.Invoke(_mainForm, new[] { message, enumValue });
                        return;
                    }

                    // Otherwise just call with the message
                    _logMessageMethod.Invoke(_mainForm, new object[] { message });
                    return;
                }
            }

            // Fallback - just log to Debug
            Console.WriteLine(message);
        }
        catch (Exception ex)
        {
            // On error, log through Debug
            Console.WriteLine($"Error logging to form: {ex.Message}");
            Console.WriteLine(message);
        }
    }

    /// <summary>
    ///     Ensures the IsSilent static field in the SqlServerToSqLite class is set to false
    /// </summary>
    private static void SetSilentFieldToFalse()
    {
        try
        {
            // Use reflection to access and modify the IsSilent property
            var type = typeof(SqlServerToSqLite);
            var field = type.GetField("IsSilent", BindingFlags.Public | BindingFlags.Static);

            if (field != null)
            {
                // Get current value
                var currentValue = (bool)field.GetValue(null);
                Console.WriteLine($"[DebugWrapper] Current IsSilent value: {currentValue}");

                // Force set to false
                field.SetValue(null, false);

                // Confirm the new value
                var newValue = (bool)field.GetValue(null);
                Console.WriteLine($"[DebugWrapper] New IsSilent value: {newValue}");
                LogToMainForm($"[Debug] Modified IsSilent flag from {currentValue} to {newValue}");
            }
            else
            {
                Console.WriteLine("[DebugWrapper] Warning: Could not find IsSilent field");
                LogToMainForm("[Debug] Warning: Could not find IsSilent field in SqlServerToSqLite class", 1);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error setting IsSilent to false: {ex.Message}");
            LogToMainForm($"[Debug] Error setting IsSilent flag: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Try to get the main form reference from the handler delegate
    /// </summary>
    private static Form GetMainFormFromHandler(SqlConversionHandler handler)
    {
        try
        {
            // Try to extract the target of the delegate which should point to our form
            if (handler.Target is Form delegateTarget)
            {
                Console.WriteLine($"[DebugWrapper] Found main form: {delegateTarget.Name}");
                return delegateTarget;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error getting main form: {ex.Message}");
        }

        return null;
    }

    /// <summary>
    ///     Get the database schema directly by querying SQL Server
    /// </summary>
    private static List<TableSchema> GetDatabaseSchema(string connectionString)
    {
        try
        {
            Console.WriteLine("[DebugWrapper] Directly querying database schema");

            var schema = new List<TableSchema>();
            using (var connection = new SqlConnection(connectionString))
            {
                connection.Open();

                // Try to call the GetDatabaseSchema method from SqlServerToSqLite if available (preferred)
                var schemaFromLibrary = GetSchemaFromLibrary(connection);
                if (schemaFromLibrary != null && schemaFromLibrary.Count > 0)
                {
                    LogToMainForm($"[Debug] Successfully retrieved schema using library method: {schemaFromLibrary.Count} tables");
                    return schemaFromLibrary;
                }

                // Fallback: Get basic table information
                var sql = @"
                    SELECT 
                        t.name AS TableName,
                        s.name AS SchemaName
                    FROM 
                        sys.tables t
                        JOIN sys.schemas s ON t.schema_id = s.schema_id
                    ORDER BY 
                        s.name, t.name";

                using (var command = new SqlCommand(sql, connection))
                {
                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            var tableName = reader["TableName"].ToString();
                            var schemaName = reader["SchemaName"].ToString();

                            var table = new TableSchema
                            {
                                TableName = tableName,
                                TableSchemaName = schemaName,
                                Columns = new List<ColumnSchema>(),
                                PrimaryKey = new List<string>(),
                                ForeignKeys = new List<ForeignKeySchema>(),
                                Indexes = new List<IndexSchema>()
                            };

                            schema.Add(table);
                        }
                    }
                }

                // For each table, fetch basic column info
                foreach (var table in schema)
                {
                    GetColumnsForTable(connection, table);
                    GetPrimaryKeyForTable(connection, table);
                    GetForeignKeysForTable(connection, table);
                }
            }

            Console.WriteLine($"[DebugWrapper] Found {schema.Count} tables via direct query");
            LogToMainForm($"[Debug] Found {schema.Count} tables via direct SQL query");
            return schema;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error querying database schema: {ex.Message}");
            LogToMainForm($"[Debug] Error querying database schema: {ex.Message}", 2);
            return new List<TableSchema>();
        }
    }

    /// <summary>
    ///     Get columns for a specific table
    /// </summary>
    private static void GetColumnsForTable(SqlConnection connection, TableSchema table)
    {
        try
        {
            var sql = @"
                SELECT 
                    c.name AS ColumnName,
                    t.name AS DataType,
                    c.max_length AS MaxLength,
                    c.is_nullable AS IsNullable,
                    c.is_identity AS IsIdentity,
                    object_definition(c.default_object_id) AS DefaultValue
                FROM 
                    sys.columns c
                    JOIN sys.types t ON c.user_type_id = t.user_type_id
                    JOIN sys.tables tbl ON c.object_id = tbl.object_id
                    JOIN sys.schemas s ON tbl.schema_id = s.schema_id
                WHERE 
                    tbl.name = @TableName AND s.name = @SchemaName
                ORDER BY 
                    c.column_id";

            using var command = new SqlCommand(sql, connection);
            command.Parameters.AddWithValue("@TableName", table.TableName);
            command.Parameters.AddWithValue("@SchemaName", table.TableSchemaName);

            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                var columnName = reader["ColumnName"].ToString();
                var dataType = reader["DataType"].ToString();
                var maxLength = Convert.ToInt32(reader["MaxLength"]);
                var isNullable = Convert.ToBoolean(reader["IsNullable"]);
                var isIdentity = Convert.ToBoolean(reader["IsIdentity"]);
                var defaultValue = reader["DefaultValue"] as string;

                var column = new ColumnSchema
                {
                    ColumnName = columnName,
                    ColumnType = dataType,
                    Length = maxLength,
                    IsNullable = isNullable,
                    IsIdentity = isIdentity,
                    DefaultValue = defaultValue
                };

                table.Columns.Add(column);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error getting columns for table {table.TableName}: {ex.Message}");
            LogToMainForm($"[Debug] Error getting columns for table {table.TableName}: {ex.Message}", 2);
        }
    }

    /// <summary>
    ///     Get primary key information for a table
    /// </summary>
    private static void GetPrimaryKeyForTable(SqlConnection connection, TableSchema table)
    {
        try
        {
            const string sql = @"
                SELECT 
                    column_name
                FROM 
                    INFORMATION_SCHEMA.KEY_COLUMN_USAGE
                WHERE 
                    OBJECTPROPERTY(OBJECT_ID(constraint_name), 'IsPrimaryKey') = 1
                    AND table_name = @TableName
                    AND table_schema = @SchemaName
                ORDER BY 
                    ordinal_position";

            using var command = new SqlCommand(sql, connection);
            command.Parameters.AddWithValue("@TableName", table.TableName);
            command.Parameters.AddWithValue("@SchemaName", table.TableSchemaName);

            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                var columnName = reader["column_name"].ToString();
                table.PrimaryKey.Add(columnName);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error getting primary key for table {table.TableName}: {ex.Message}");
        }
    }

    /// <summary>
    ///     Get foreign key information for a table
    /// </summary>
    private static void GetForeignKeysForTable(SqlConnection connection, TableSchema table)
    {
        try
        {
            const string sql = @"
                SELECT 
                    fk.name AS FK_Name,
                    c.name AS ColumnName,
                    rt.name AS ReferenceTableName,
                    rc.name AS ReferenceColumnName,
                    fk.delete_referential_action AS DeleteAction,
                    c.is_nullable AS IsNullable
                FROM 
                    sys.foreign_keys fk
                    INNER JOIN sys.foreign_key_columns fkc ON fk.object_id = fkc.constraint_object_id
                    INNER JOIN sys.tables t ON t.object_id = fk.parent_object_id
                    INNER JOIN sys.tables rt ON rt.object_id = fk.referenced_object_id
                    INNER JOIN sys.columns c ON fkc.parent_column_id = c.column_id AND c.object_id = t.object_id
                    INNER JOIN sys.columns rc ON fkc.referenced_column_id = rc.column_id AND rc.object_id = rt.object_id
                WHERE 
                    t.name = @TableName
                    AND SCHEMA_NAME(t.schema_id) = @SchemaName";

            using var command = new SqlCommand(sql, connection);
            command.Parameters.AddWithValue("@TableName", table.TableName);
            command.Parameters.AddWithValue("@SchemaName", table.TableSchemaName);

            using var reader = command.ExecuteReader();
            while (reader.Read())
            {
                var fk = new ForeignKeySchema
                {
                    TableName = table.TableName,
                    ColumnName = reader["ColumnName"].ToString(),
                    ForeignTableName = reader["ReferenceTableName"].ToString(),
                    ForeignColumnName = reader["ReferenceColumnName"].ToString(),
                    CascadeOnDelete = Convert.ToInt32(reader["DeleteAction"]) == 1, // 1 = CASCADE
                    IsNullable = Convert.ToBoolean(reader["IsNullable"])
                };

                table.ForeignKeys.Add(fk);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error getting foreign keys for table {table.TableName}: {ex.Message}");
        }
    }

    /// <summary>
    ///     Try to use the library's method to get the schema if possible
    /// </summary>
    private static List<TableSchema> GetSchemaFromLibrary(SqlConnection connection)
    {
        try
        {
            // Try to use reflection to call the library's GetDatabaseSchema method
            var type = typeof(SqlServerToSqLite);
            var method = type.GetMethod("GetDatabaseSchema",
                BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static);

            if (method != null)
            {
                Console.WriteLine("[DebugWrapper] Using library's GetDatabaseSchema method");
                return method.Invoke(null, new object[] { connection }) as List<TableSchema>;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"[DebugWrapper] Error using library's GetDatabaseSchema: {ex.Message}");
            LogToMainForm($"[Debug] Error using library's GetDatabaseSchema method: {ex.Message}", 2);
        }

        return null;
    }
}