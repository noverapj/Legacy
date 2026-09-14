using log4net.Config;
using System.IO;

// Configure LOG4NET Using configuration file.
[assembly: log4net.Config.XmlConfigurator(Watch = true)]

namespace ApplicationForm
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // Setup basic logging configuration
            BasicConfigurator.Configure();
            
            // Setup default log file path in AppData
            var appDataPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            var appFolder = Path.Combine(appDataPath, "SQLServerToSQLite");
            
            // Ensure log directory exists
            if (!Directory.Exists(appFolder))
                Directory.CreateDirectory(appFolder);
            
            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new MainForm());
        }
    }
}