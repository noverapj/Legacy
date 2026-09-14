#region

using System.Text.RegularExpressions;
using log4net;

#endregion

namespace ClassLibrary;

/// <summary>
///     This class is responsible to take a single SQL Server database
///     and convert it to an SQLite database file.
/// </summary>
/// <remarks>The class knows how to convert table and index structures only.</remarks>
public class SqlServerToSqLite
{
    #region Public Properties

    /// <summary>
    ///     Gets a value indicating whether this instance is active.
    /// </summary>
    /// <value><c>true</c> if this instance is active; otherwise, <c>false</c>.</value>
    public static bool IsActive { get; private set; }

    #endregion

    /// <summary>
    ///     Gets a create script for the triggerSchema in sqlite syntax
    /// </summary>
    /// <param name="ts">Trigger to script</param>
    /// <returns>Executable script</returns>
    public static string WriteTriggerSchema(TriggerSchema ts)
    {
        return $"CREATE TRIGGER [{ts.Name}] {ts.Type} {ts.Event} ON [{ts.Table}] BEGIN {ts.Body} END;";
    }

    #region Public Methods

    /// <summary>
    ///     Cancels the conversion process.
    /// </summary>
    public static void CancelConversion()
    {
        _cancelled = true;
    }

    /// <summary>
    ///     Indicates whether the conversion process should run silently.
    /// </summary>
    public static bool IsSilent;

    /// <summary>
    ///     Initializes a new instance of the <see cref="SqlServerToSqLite"/> class.
    /// </summary>
    /// <param name="isSil">Indicates whether the conversion process should run silently.</param>
    /// <param name="debugOutput">Indicates whether debug output should be enabled.</param>
    /// <param name="timestampAsInt64">Indicates whether timestamps should be treated as Int64.</param>
    public SqlServerToSqLite(bool isSil = true, bool debugOutput = false, bool timestampAsInt64 = false)
    {
        IsSilent = isSil;
        EnableDebugOutput = debugOutput;
        TreatTimestampAsInt64 = timestampAsInt64;
    }

    /// <summary>
    ///     Indicates whether debug output is enabled.
    /// </summary>
    public static bool EnableDebugOutput;

    /// <summary>
    ///     Indicates whether timestamps should be treated as Int64.
    /// </summary>
    public static bool TreatTimestampAsInt64;

    /// <summary>
    ///     Converts a SQL Server database to an SQLite database file.
    /// </summary>
    /// <param name="sqlServerConnString">The connection string to the SQL Server database.</param>
    /// <param name="sqlitePath">The path to the SQLite database file that needs to get created.</param>
    /// <param name="password">The password to use or NULL if no password should be used to encrypt the DB.</param>
    /// <param name="handler">A handler delegate for progress notifications.</param>
    /// <param name="selectionHandler">The selection handler that allows the user to select which tables to convert.</param>
    /// <param name="viewFailureHandler">A handler for managing failed view definitions.</param>
    /// <param name="createTriggers">Indicates whether triggers should be created.</param>
    /// <param name="createViews">Indicates whether views should be created.</param>
    /// <param name="treatGuidAsString">Indicates whether GUIDs should be treated as strings.</param>
    /// <param name="isSil">Indicates whether the conversion process should run silently.</param>
    /// <param name="debugOutput">Indicates whether debug output should be enabled.</param>
    /// <param name="treatTimestampAsInt64">Indicates whether timestamps should be treated as Int64.</param>
    /// <remarks>
    ///     The method continues asynchronously in the background and the caller is returned immediately.
    /// </remarks>
    public static void ConvertSqlServerToSqLiteDatabase(string sqlServerConnString,
        string sqlitePath, string password, SqlConversionHandler handler,
        SqlTableSelectionHandler selectionHandler,
        FailedViewDefinitionHandler viewFailureHandler,
        bool createTriggers, bool createViews, bool treatGuidAsString, bool isSil,
        bool debugOutput = false, bool treatTimestampAsInt64 = false)
    {
        IsSilent = isSil;
        EnableDebugOutput = debugOutput;
        TreatTimestampAsInt64 = treatTimestampAsInt64;

        // Clear cancelled flag
        _cancelled = false;

        try
        {
            IsActive = true;
            ConvertSqlServerDatabaseToSqLiteFile(sqlServerConnString, sqlitePath, password, handler,
                selectionHandler, viewFailureHandler, createTriggers, createViews, treatGuidAsString);
            IsActive = false;
            if (IsSilent != true) handler(true, true, 100, "Finished converting database");
        }
        catch (Exception ex)
        {
            _log.Error("Failed to convert SQL Server database to SQLite database", ex);
            IsActive = false;
            if (IsSilent != true) handler(true, false, 100, ex.Message);
        }
    }

    #endregion

    #region Private Methods

    /// <summary>
    ///     Converts the SQL Server database schema and data to an SQLite database file.
    /// </summary>
    /// <param name="sqlConnString">The SQL Server connection string.</param>
    /// <param name="sqlitePath">The path to the generated SQLite database file.</param>
    /// <param name="password">The password to use for encrypting the DB or null if none is needed.</param>
    /// <param name="handler">A handler for progress notifications.</param>
    /// <param name="selectionHandler">The selection handler which allows the user to select which tables to convert.</param>
    /// <param name="viewFailureHandler">A handler for managing failed view definitions.</param>
    /// <param name="createTriggers">Indicates whether triggers should be created.</param>
    /// <param name="createViews">Indicates whether views should be created.</param>
    /// <param name="treatGuidAsString">Indicates whether GUIDs should be treated as strings.</param>
    private static void ConvertSqlServerDatabaseToSqLiteFile(
        string sqlConnString, string sqlitePath, string password, SqlConversionHandler handler,
        SqlTableSelectionHandler selectionHandler,
        FailedViewDefinitionHandler viewFailureHandler,
        bool createTriggers, bool createViews, bool treatGuidAsString)
    {
        // Implementation omitted for brevity
    }

    #endregion

    #region Private Variables

    /// <summary>
    ///     Indicates whether the conversion process has been cancelled.
    /// </summary>
    private static bool _cancelled;

    /// <summary>
    ///     Regex for parsing index keys.
    /// </summary>
    private static readonly Regex _keyRx = new(@"(([a-zA-Z_äöüÄÖÜß0-9\.]|(\s+))+)(\(\-\))?");

    /// <summary>
    ///     Regex for parsing default values.
    /// </summary>
    private static readonly Regex _defaultValueRx = new(@"\(N(\'.*\')\)");

    /// <summary>
    ///     Logger instance for logging errors and debug information.
    /// </summary>
    private static readonly ILog _log = LogManager.GetLogger(typeof(SqlServerToSqLite));

    #endregion
}

/// <summary>
///     This handler is called whenever progress is made in the conversion process.
/// </summary>
/// <param name="done">TRUE indicates that the entire conversion process is finished.</param>
/// <param name="success">TRUE indicates that the current step finished successfully.</param>
/// <param name="percent">Progress percent (0-100)</param>
/// <param name="msg">A message that accompanies the progress.</param>
public delegate void SqlConversionHandler(bool done, bool success, int percent, string msg);

/// <summary>
///     This handler allows the user to change which tables get converted from SQL Server
///     to SQLite.
/// </summary>
/// <param name="schema">The original SQL Server DB schema</param>
/// <returns>The same schema minus any table we don't want to convert.</returns>
public delegate List<TableSchema> SqlTableSelectionHandler(List<TableSchema> schema);

/// <summary>
///     This handler is called in order to handle the case when copying the SQL Server view SQL
///     statement is not enough and the user needs to either update the view definition himself
///     or discard the view definition from the generated SQLite database.
/// </summary>
/// <param name="vs">The problematic view definition</param>
/// <returns>The updated view definition, or NULL in case the view should be discarded</returns>
public delegate string FailedViewDefinitionHandler(ViewSchema vs);