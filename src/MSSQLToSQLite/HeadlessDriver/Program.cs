using System.Diagnostics;
using ApplicationForm;
using ClassLibrary;

// Headless driver for SqlConversionDebugWrapper (GUI-free conversion)
// Usage: HeadlessDriver.exe "<mssql conn string>" "<output sqlite path>"

if (args.Length < 2)
{
    Console.WriteLine("Usage: HeadlessDriver.exe \"<mssql conn string>\" \"<output sqlite path>\"");
    return 1;
}

string connString = args[0];
string sqlitePath = Path.GetFullPath(args[1]);

Console.WriteLine($"Source : {connString}");
Console.WriteLine($"Target : {sqlitePath}");

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
        false,              // create views: no
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
