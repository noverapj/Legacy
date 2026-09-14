using System.Diagnostics;
using ApplicationForm;
using ClassLibrary;

// Headless driver for SqlConversionDebugWrapper (GUI-free conversion)
// Usage: HeadlessDriver.exe "<mssql conn string>" "<output sqlite path>" [views=true|false]

if (args.Length < 2)
{
    Console.WriteLine("Usage: HeadlessDriver.exe \"<mssql conn string>\" \"<output sqlite path>\" [views=true|false]");
    return 1;
}

string connString = args[0];
string sqlitePath = Path.GetFullPath(args[1]);
bool includeViews = args.Length < 3 || !string.Equals(args[2], "false", StringComparison.OrdinalIgnoreCase);

Console.WriteLine($"Source : {connString}");
Console.WriteLine($"Target : {sqlitePath}");
Console.WriteLine($"Views  : {(includeViews ? "yes" : "no")}");

SqlConversionHandler handler = (done, success, percent, msg) =>
{
    string tag = done ? (success ? "[DONE-OK]" : "[DONE-FAIL]") : "[....]";
    Console.WriteLine($"{tag} {percent,3}% {msg}");
};

SqlTableSelectionHandler selectAllTables = schema =>
{
    Console.WriteLine($"Selecting ALL {schema.Count} tables for conversion");
    return schema;
};

try
{
    SqlConversionDebugWrapper.ConvertWithDebug(
        connString,
        sqlitePath,
        null,               // no db password
        handler,
        selectAllTables,    // convert every table
        null,               // no view failure handler (views disabled)
        true,              // create triggers
        includeViews,       // create views
        true,              // treat GUIDs as strings
        false,              // silent
        false,              // debug output
        false);             // timestamp as int64

    Console.WriteLine("Conversion finished.");
    return 0;
}
catch (Exception ex)
{
    Console.WriteLine($"FATAL: {ex}");
    return 2;
}
