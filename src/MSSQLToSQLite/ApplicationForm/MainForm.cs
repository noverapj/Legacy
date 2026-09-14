#region

using System.Data.SqlClient;
using System.Diagnostics;
using System.Reflection;
using System.Text;
using ClassLibrary;
using Microsoft.Win32;
using MethodInvoker = System.Windows.Forms.MethodInvoker;

#endregion

namespace ApplicationForm;

public partial class MainForm : Form
{
    #region Private Variables

    private bool _shouldExit;
    private string _logFilePath;
    private bool _enableLogging;

    #endregion

    #region Constructor

    public MainForm()
    {
        InitializeComponent();
        LoadSqlServerInstances();
        InitLogging();
        
        // Set up event handler for the checkbox (not added in designer)
        cbxEnableLogging.CheckedChanged += cbxEnableLogging_CheckedChanged;
    }

    #endregion

    #region Event Handler

    private void btnBrowseSQLitePath_Click(object sender, EventArgs e)
    {
        var res = saveFileDialog1.ShowDialog(this);
        if (res == DialogResult.Cancel)
            return;

        var sqliteFilepath = saveFileDialog1.FileName;

        // Make sure the file has the proper extension based on the filter selection
        var extension = Path.GetExtension(sqliteFilepath).ToLowerInvariant();
        if (string.IsNullOrEmpty(extension) || (extension != ".sqlite" && extension != ".db"))
            // If no extension was provided or an invalid extension, add the default extension
            sqliteFilepath = Path.ChangeExtension(sqliteFilepath, saveFileDialog1.DefaultExt);

        txtSQLitePath.Text = sqliteFilepath;
        SaveSqlitePath();
        pbrProgress.Value = 0;
        lblMessage.Text = string.Empty;
    }

    private void cboDatabases_SelectedIndexChanged(object sender, EventArgs e)
    {
        UpdateSensitivity();
        pbrProgress.Value = 0;
        lblMessage.Text = string.Empty;
    }

    private void btnSet_Click(object sender, EventArgs e)
    {
        try
        {
            if (cboSqlInstances.SelectedItem == null)
            {
                MessageBox.Show(this,@"Please select a SQL Server instance first", @"No SQL Server Selected", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            var serverAddress = cboSqlInstances.SelectedItem.ToString();
            if (serverAddress != null)
            {
                var connString = cbxIntegrated.Checked ? GetSqlServerConnectionString(serverAddress, "master") : GetSqlServerConnectionString(serverAddress, "master", txtUserDB.Text, txtPassDB.Text);
                using var conn = new SqlConnection(connString);
                conn.Open();

                // Get the names of all DBs in the database server.
                var query = new SqlCommand(@"select distinct [name] from sysdatabases", conn);
                using var reader = query.ExecuteReader();
                cboDatabases.Items.Clear();
                while (reader.Read())
                    cboDatabases.Items.Add((string)reader[0]);
                if (cboDatabases.Items.Count > 0)
                    cboDatabases.SelectedIndex = 0;
            }

            cboDatabases.Enabled = true;

            pbrProgress.Value = 0;
            lblMessage.Text = string.Empty;
        }
        catch (Exception ex)
        {
            MessageBox.Show(this,
                ex.Message,
                @"Failed To Connect",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error);
        } // catch
    }

    private void txtSQLitePath_TextChanged(object sender, EventArgs e)
    {
        UpdateSensitivity();
    }

    private void MainForm_Load(object sender, EventArgs e)
    {
        UpdateSensitivity();

        var version = Assembly.GetExecutingAssembly().GetName().Version?.ToString();
        Text = @"SQL Server To SQLite DB Converter (" + version + ")";
    }

    private void btnCancel_Click(object sender, EventArgs e)
    {
        SqlServerToSqLite.CancelConversion();
    }

    private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
    {
        if (SqlServerToSqLite.IsActive)
        {
            SqlServerToSqLite.CancelConversion();
            _shouldExit = true;
            e.Cancel = true;
        }
        else
        {
            e.Cancel = false;
        }
    }

    private void cbxEncrypt_CheckedChanged(object sender, EventArgs e)
    {
        UpdateSensitivity();
    }

    private void txtPassword_TextChanged(object sender, EventArgs e)
    {
        UpdateSensitivity();
    }

    private void ChkIntegratedCheckedChanged(object sender, EventArgs e)
    {
        if (cbxIntegrated.Checked)
        {
            lblPassword.Visible = false;
            lblUser.Visible = false;
            txtPassDB.Visible = false;
            txtUserDB.Visible = false;
        }
        else
        {
            lblPassword.Visible = true;
            lblUser.Visible = true;
            txtPassDB.Visible = true;
            txtUserDB.Visible = true;
        }
    }

    private void btnStart_Click(object sender, EventArgs e)
    {
        // Validate required selections
        if (!ValidateSelections()) return;

        // Save settings and get configuration
        SaveSqlitePath();
        var config = GetConversionConfig();
        if (config == null) return;

        // Log configuration
        LogConversionConfig(config);
        
        // Start conversion with visual feedback
        Cursor = Cursors.WaitCursor;
        LogMessage("Starting conversion process...", LogLevel.Info);
        
        // Ensure dialogs will show during conversion
        SqlServerToSqLiteFixer.EnsureDialogsWillShow();
        LogMessage("Forced dialog display mode enabled", LogLevel.Info);

                // Show the log tab if logging is enabled
        if (_enableLogging && tabControl1.SelectedIndex != 1)
        {
            // Switch to log tab to show the user it's active
            tabControl1.SelectedIndex = 1;
        }
        
        // Run the conversion using our debug wrapper
        SqlConversionDebugWrapper.ConvertWithDebug(
            config.SqlConnString,
            config.SqlitePath,
            config.Password,
            CreateProgressHandler(),
            CreateTableSelectionHandler(),
            CreateViewFailureHandler(),
            config.CreateTriggers,
            config.CreateViews,
            config.TreatGuidAsString,
            false,  // Force silent to false
            true,   // Enable debug output
            false); // Don't treat timestamp as Int64
    }

    /// <summary>
    /// Validates that all required selections are made before starting conversion
    /// </summary>
    private bool ValidateSelections()
    {
        if (cboSqlInstances.SelectedItem == null)
        {
            MessageBox.Show(this,
                @"Please select a SQL Server instance first",
                @"No SQL Server Selected",
                MessageBoxButtons.OK,
                MessageBoxIcon.Warning);
            return false;
        }

        var selectedDb = cboDatabases.SelectedItem?.ToString();
        if (string.IsNullOrEmpty(selectedDb))
        {
            LogMessage("No database selected. Please select a database.", LogLevel.Error);
            return false;
        }

        return true;
    }

    /// <summary>
    /// Class to hold conversion configuration parameters
    /// </summary>
    private class ConversionConfig
    {
        public string SqlConnString { get; set; }
        public string SqlitePath { get; set; }
        public string Password { get; set; }
        public bool CreateViews { get; set; }
        public bool CreateTriggers { get; set; }
        public bool TreatGuidAsString { get; set; }
        public string ServerName { get; set; }
        public string DatabaseName { get; set; }
    }

    /// <summary>
    /// Gathers all configuration parameters for the conversion
    /// </summary>
    private ConversionConfig GetConversionConfig()
    {
        var serverAddress = cboSqlInstances.SelectedItem.ToString();
        var selectedDb = cboDatabases.SelectedItem?.ToString();
        
        string sqlConnString;
        if (cbxIntegrated.Checked)
        {
            sqlConnString = GetSqlServerConnectionString(serverAddress, selectedDb);
        }
        else
        {
            sqlConnString = GetSqlServerConnectionString(serverAddress, selectedDb,
                txtUserDB.Text, txtPassDB.Text);
        }

        var password = txtPassword.Text.Trim();
        if (!cbxEncrypt.Checked)
        {
            password = null;
        }

        return new ConversionConfig
        {
            SqlConnString = sqlConnString,
            SqlitePath = txtSQLitePath.Text.Trim(),
            Password = password,
            CreateViews = cbxCreateViews.Checked,
            CreateTriggers = cbxTriggers.Checked,
            TreatGuidAsString = chkBox_treatGuidAsString.Checked,
            ServerName = serverAddress,
            DatabaseName = selectedDb
        };
    }

    /// <summary>
    /// Logs the configuration settings being used
    /// </summary>
    private void LogConversionConfig(ConversionConfig config)
    {
        LogMessage($"Starting conversion from {config.ServerName}/{config.DatabaseName} to SQLite", LogLevel.Info);
        
        if (cbxIntegrated.Checked)
        {
            LogMessage("Using integrated security for SQL Server connection", LogLevel.Info);
        }
        else
        {
            LogMessage("Using SQL Server authentication", LogLevel.Info);
        }

        LogMessage($"SQLite target file: {config.SqlitePath}", LogLevel.Info);
        LogMessage($"Options: Create Views={config.CreateViews}, Create Triggers={config.CreateTriggers}, Treat GUID as String={config.TreatGuidAsString}", LogLevel.Info);
        
        if (config.Password != null)
        {
            LogMessage("Using encryption for SQLite database", LogLevel.Info);
        }
    }

    /// <summary>
    /// Creates a handler for progress updates during conversion
    /// </summary>
    private SqlConversionHandler CreateProgressHandler()
    {
        return delegate(bool done, bool success, int percent, string msg)
        {
            Invoke(new MethodInvoker(delegate()
            {
                UpdateSensitivity();
                lblMessage.Text = msg;
                pbrProgress.Value = percent;

                // Determine appropriate log level
                LogLevel logLevel = done 
                    ? (success ? LogLevel.Success : LogLevel.Error) 
                    : LogLevel.Info;
                
                LogMessage(msg, logLevel);

                // When finished, restore UI state
                if (done)
                {
                    btnStart.Enabled = true;
                    Cursor = Cursors.Default;
                    UpdateSensitivity();

                    if (success)
                    {
                        ShowSuccessMessage(msg);
                    }
                    else if (!_shouldExit)
                    {
                        ShowFailureMessage(msg);
                    }
                    else
                    {
                        Application.Exit();
                    }
                }
            }));
        };
    }

    /// <summary>
    /// Creates a handler for table selection during conversion
    /// </summary>
    private SqlTableSelectionHandler CreateTableSelectionHandler()
    {
        return delegate(List<TableSchema> schema)
        {
            List<TableSchema> updated = null;
            Invoke(new MethodInvoker(delegate
            {
                // Log the found tables
                LogMessage($"Found {schema.Count} tables in database:", LogLevel.Info);
                LogTableNames(schema);
                
                // Show table selection dialog
                updated = ShowTableSelectionDialog(schema);
            }));

            // Update UI with table count
            if (updated != null)
            {
                Invoke(new MethodInvoker(delegate { 
                    lblMessage.Text = $"Converting {updated.Count} tables..."; 
                }));
            }

            return updated;
        };
    }

    /// <summary>
    /// Logs the names of tables found in the database
    /// </summary>
    private void LogTableNames(List<TableSchema> tables)
    {
        foreach (var table in tables)
        {
            LogMessage($"  - Table: {table.TableName} ({table.Columns?.Count ?? 0} columns)", LogLevel.Info);
        }
    }

    /// <summary>
    /// Shows the table selection dialog and returns the selected tables
    /// </summary>
    private List<TableSchema> ShowTableSelectionDialog(List<TableSchema> schema)
    {
        var dlg = new TableSelectionDialog();
        var res = dlg.ShowTables(schema, this);

        if (res == DialogResult.OK)
        {
            // User selected tables and clicked OK
            var selected = dlg.IncludedTables;
            
            if (selected != null && selected.Count > 0)
            {
                LogMessage($"Selected {selected.Count} tables for conversion:", LogLevel.Info);
                foreach (var table in selected)
                {
                    LogMessage($"  - Converting: {table.TableName}", LogLevel.Info);
                }
                return selected;
            }
            
            LogMessage("No tables were selected, using all tables", LogLevel.Warning);
        }
        else
        {
            // If user cancelled or closed the dialog, default to all tables
            LogMessage("Table selection canceled, using all tables", LogLevel.Info);
        }
        
        return schema;
    }

    /// <summary>
    /// Creates a handler for view definition failures during conversion
    /// </summary>
    private FailedViewDefinitionHandler CreateViewFailureHandler()
    {
        return delegate(ViewSchema vs)
        {
            string updated = null;
            Invoke(new MethodInvoker(delegate
            {
                LogMessage($"View definition failed for view: {vs.ViewName}", LogLevel.Warning);
                var dlg = new ViewFailureDialog();
                dlg.View = vs;
                var res = dlg.ShowDialog(this);
                if (res == DialogResult.OK)
                {
                    updated = dlg.ViewSQL;
                    LogMessage($"User provided fixed view definition for: {vs.ViewName}", LogLevel.Info);
                }
                else
                {
                    updated = null;
                    LogMessage($"View {vs.ViewName} will be skipped", LogLevel.Warning);
                }
            }));

            return updated;
        };
    }

    /// <summary>
    /// Shows a success message when conversion completes
    /// </summary>
    private void ShowSuccessMessage(string message)
    {
        MessageBox.Show(this,
            message,
            @"Conversion Finished",
            MessageBoxButtons.OK,
            MessageBoxIcon.Information);
        pbrProgress.Value = 0;
        lblMessage.Text = string.Empty;
        LogMessage("Conversion completed successfully", LogLevel.Success);
    }

    /// <summary>
    /// Shows a failure message when conversion fails
    /// </summary>
    private void ShowFailureMessage(string message)
    {
        MessageBox.Show(this,
            message,
            @"Conversion Failed",
            MessageBoxButtons.OK,
            MessageBoxIcon.Error);
        pbrProgress.Value = 0;
        lblMessage.Text = string.Empty;
        LogMessage($"Conversion failed: {message}", LogLevel.Error);
    }

    private void cboSqlInstances_SelectedIndexChanged(object sender, EventArgs e)
    {
        UpdateSensitivity();
        
        // Automatically load databases when server is selected
        if (cboSqlInstances.SelectedItem != null && !SqlServerToSqLite.IsActive)
        {
            LoadDatabases();
        }
    }

    private void cbxEnableLogging_CheckedChanged(object sender, EventArgs e)
    {
        _enableLogging = cbxEnableLogging.Checked;
        
        // Save the setting to user preferences
        try
        {
            Properties.Settings.Default.EnableLogging = _enableLogging;
            Properties.Settings.Default.Save();
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"Failed to save logging setting: {ex.Message}");
        }
        
        LogMessage(_enableLogging ? "Logging to file enabled" : "Logging to file disabled", LogLevel.Info);
        

    }

    #endregion

    #region Private Methods

    private void InitLogging()
    {
        // Set up default log file path
        var appDataPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        var appFolder = Path.Combine(appDataPath, "SQLServerToSQLite");
        _logFilePath = Path.Combine(appFolder, "converter.log");
        
        // Configure the RichTextBox for better performance
        rtLog.Clear();
        rtLog.HideSelection = false;
        rtLog.DetectUrls = false;
        rtLog.WordWrap = true;
        rtLog.ScrollBars = RichTextBoxScrollBars.Both;
        rtLog.BackColor = Color.White;
        
        // Set a default font for better readability
        rtLog.Font = new Font("Consolas", 9F, FontStyle.Regular, GraphicsUnit.Point);
        
        // Add event handler for tab changes to refresh the log display
        tabControl1.SelectedIndexChanged += TabControl1_SelectedIndexChanged;
        
        // Check if we have a saved log file path in settings
        LoadSettings();
        
        // Initial log message
        LogMessage($"Application started. Version: {Assembly.GetExecutingAssembly().GetName().Version}", LogLevel.Info);
        LogMessage($"Log file: {_logFilePath}", LogLevel.Info);
    }

    /// <summary>
    /// Refresh the log display when switching to the log tab
    /// </summary>
    private void TabControl1_SelectedIndexChanged(object sender, EventArgs e)
    {
        // If switching to the log tab, refresh the display
        if (tabControl1.SelectedTab == tabPage2 && rtLog != null)
        {
            RefreshLogDisplay();
        }
    }

    /// <summary>
    /// Refreshes the log display
    /// </summary>
    private void RefreshLogDisplay()
    {
        if (rtLog != null && !IsDisposed)
        {
            try
            {
                rtLog.SuspendLayout();
                // Force a redraw by doing a small edit and then undoing it
                int originalPosition = rtLog.SelectionStart;
                rtLog.SelectionStart = rtLog.TextLength;
                rtLog.SelectionLength = 0;
                rtLog.ScrollToCaret();
                rtLog.Refresh();
                rtLog.Update();
                // Restore position
                rtLog.SelectionStart = originalPosition;
                rtLog.ResumeLayout();
            }
            catch
            {
                // Ignore errors that might occur during shutdown
            }
        }
    }

    private void SaveSqlitePath()
    {
        var path = txtSQLitePath.Text.Trim();
        try
        {
            if (string.IsNullOrEmpty(path)) return;
            // Save the path to user settings
            Properties.Settings.Default.LastSQLitePath = path;
            Properties.Settings.Default.Save();
            LogMessage($"Saved SQLite path: {path}", LogLevel.Info);
        }
        catch (Exception ex)
        {
            LogMessage($"Failed to save path: {ex.Message}", LogLevel.Error);
        }
    }

    private void LoadSettings()
    {
        var lastPath = Properties.Settings.Default.LastSQLitePath;
        try
        {
            // Load the last used SQLite path
            if (!string.IsNullOrEmpty(lastPath))
            {
                txtSQLitePath.Text = lastPath;
                LogMessage($"Loaded last used SQLite path: {lastPath}", LogLevel.Info);
            }
            
            // Load logging preferences
            _enableLogging = Properties.Settings.Default.EnableLogging;
            cbxEnableLogging.Checked = _enableLogging;
        }
        catch (Exception ex)
        {
            LogMessage($"Failed to load settings: {ex.Message}", LogLevel.Error);
        }
    }

    private void UpdateSensitivity()
    {
        if (txtSQLitePath.Text.Trim().Length > 0 && cboDatabases.Enabled &&
            (!cbxEncrypt.Checked || txtPassword.Text.Trim().Length > 0))
            btnStart.Enabled = true && !SqlServerToSqLite.IsActive;
        else
            btnStart.Enabled = false;

        btnSet.Enabled = cboSqlInstances.SelectedItem != null && !SqlServerToSqLite.IsActive;
        btnCancel.Visible = SqlServerToSqLite.IsActive;
        cboSqlInstances.Enabled = !SqlServerToSqLite.IsActive;
        txtSQLitePath.Enabled = !SqlServerToSqLite.IsActive;
        btnBrowseSQLitePath.Enabled = !SqlServerToSqLite.IsActive;
        cbxEncrypt.Enabled = !SqlServerToSqLite.IsActive;
        cboDatabases.Enabled = cboDatabases.Items.Count > 0 && !SqlServerToSqLite.IsActive;
        txtPassword.Enabled = cbxEncrypt.Checked && cbxEncrypt.Enabled;
        cbxIntegrated.Enabled = !SqlServerToSqLite.IsActive;
        cbxCreateViews.Enabled = !SqlServerToSqLite.IsActive;
        cbxTriggers.Enabled = !SqlServerToSqLite.IsActive;
        txtPassDB.Enabled = !SqlServerToSqLite.IsActive;
        txtUserDB.Enabled = !SqlServerToSqLite.IsActive;
    }

    private static string GetSqlServerConnectionString(string address, string db)
    {
        var res = @"Data Source=" + address.Trim() +
                  ";Initial Catalog=" + db.Trim() + ";Integrated Security=SSPI;";
        return res;
    }

    private static string GetSqlServerConnectionString(string address, string db, string user, string pass)
    {
        var res = @"Data Source=" + address.Trim() +
                  ";Initial Catalog=" + db.Trim() + ";User ID=" + user.Trim() + ";Password=" + pass.Trim();
        return res;
    }

    private void LoadSqlServerInstances()
    {
        try
        {
            cboSqlInstances.Items.Clear();
            var uniqueInstances = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

            // Add the local machine basic names that work with SQL Server
            uniqueInstances.Add(".");
            uniqueInstances.Add("(local)");
            uniqueInstances.Add("localhost");
            uniqueInstances.Add(Environment.MachineName);

            // Try to discover SQL Express and other SQL instances via registry
            AddSqlInstancesFromRegistry(uniqueInstances);

            // Check for SQL Server Browser service via direct connection
            TryFindLocalSqlServerInstancesByConnection(uniqueInstances);

            // Add named pipe options for local SQL Express (common default)
            uniqueInstances.Add($"{Environment.MachineName}\\SQLEXPRESS");
            uniqueInstances.Add(".\\SQLEXPRESS");

            // Add all found instances to the combo box
            foreach (var instance in uniqueInstances.OrderBy(i => i)) cboSqlInstances.Items.Add(instance);

            // Select the first item if available
            if (cboSqlInstances.Items.Count > 0)
            {
                cboSqlInstances.SelectedIndex = 0;
            }
            else
            {
                // If no instances found, add at least the local default
                cboSqlInstances.Items.Add(".");
                cboSqlInstances.SelectedIndex = 0;
            }
        }
        catch (Exception ex)
        {
            MessageBox.Show(this,
                @"Failed to retrieve SQL Server instances: " + ex.Message,
                @"Error",
                MessageBoxButtons.OK,
                MessageBoxIcon.Error);

            // Ensure there's at least one default option
            if (cboSqlInstances.Items.Count == 0)
            {
                cboSqlInstances.Items.Add(".");
                cboSqlInstances.SelectedIndex = 0;
            }
        }
    }

    /// <summary>
    ///     Attempt to find local SQL Server instances by direct connection
    /// </summary>
    private static void TryFindLocalSqlServerInstancesByConnection(HashSet<string> instances)
    {
        try
        {
            // Try connecting to the local default instance
            using var connection =
                new SqlConnection(
                    "Data Source=.;Initial Catalog=master;Integrated Security=SSPI;Connection Timeout=1");
            connection.Open();
            // If successful, add the local machine info
            instances.Add(".");
        }
        catch
        {
            // Ignore connection errors - this is just a detection mechanism
        }

        // Check for a few common named instances
        string[] commonInstances = { "SQLEXPRESS", "SQL2019", "SQL2022", "MSSQLSERVER", "MSSQL" };

        foreach (var instance in commonInstances)
            try
            {
                var serverName = $".\\{instance}";
                using var connection =
                    new SqlConnection(
                        $"Data Source={serverName};Initial Catalog=master;Integrated Security=SSPI;Connection Timeout=1");
                connection.Open();
                // If connection successful, add this instance
                instances.Add(serverName);
                instances.Add($"(local)\\{instance}");
                instances.Add($"localhost\\{instance}");
                instances.Add($"{Environment.MachineName}\\{instance}");
            }
            catch
            {
                // Ignore connection errors - this is just a detection mechanism
            }
    }

    /// <summary>
    ///     Checks if the server name refers to the local machine
    /// </summary>
    private bool IsLocalMachine(string serverName)
    {
        if (string.IsNullOrEmpty(serverName))
            return false;

        return serverName.Equals(".", StringComparison.OrdinalIgnoreCase) ||
               serverName.Equals("(local)", StringComparison.OrdinalIgnoreCase) ||
               serverName.Equals("localhost", StringComparison.OrdinalIgnoreCase) ||
               serverName.Equals(Environment.MachineName, StringComparison.OrdinalIgnoreCase);
    }

    /// <summary>
    ///     Retrieves SQL Server instances from the Windows Registry
    /// </summary>
    private void AddSqlInstancesFromRegistry(HashSet<string> instances)
    {
        try
        {
            // Check for instances in registry
            using var hklm = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry64);
            // Look for instance names
            using (var instanceKey = hklm.OpenSubKey(@"SOFTWARE\Microsoft\Microsoft SQL Server\Instance Names\SQL"))
            {
                if (instanceKey != null)
                    foreach (var instanceName in instanceKey.GetValueNames())
                        if (instanceName.Equals("MSSQLSERVER", StringComparison.OrdinalIgnoreCase))
                        {
                            // Default instance - just add the machine name (already added above)
                        }
                        else
                        {
                            // Named instance - add with format SERVER\INSTANCE
                            instances.Add($"{Environment.MachineName}\\{instanceName}");
                            instances.Add($".\\{instanceName}");
                            instances.Add($"(local)\\{instanceName}");
                            instances.Add($"localhost\\{instanceName}");
                        }
            }

            // Also check for SQL Server browser
            using (var sqlKey = hklm.OpenSubKey(@"SOFTWARE\Microsoft\Microsoft SQL Server"))
            {
                if (sqlKey == null) return;
                foreach (var name in sqlKey.GetSubKeyNames())
                    if (name.StartsWith("MSSQL"))
                    {
                        using var instanceKey = sqlKey.OpenSubKey(name + @"\MSSQLServer\SuperSocketNetLib\Tcp");
                        if (instanceKey == null) continue;
                        var tcpPort = instanceKey.GetValue("TcpPort") as string;
                        if (string.IsNullOrEmpty(tcpPort)) continue;
                        // Add specific port connection option
                        var serverName = $"{Environment.MachineName},{tcpPort}";
                        instances.Add(serverName);
                        instances.Add($"localhost,{tcpPort}");
                    }
            }
        }
        catch (Exception ex)
        {
            // Just log or ignore registry errors - we have other methods to find instances
            Debug.WriteLine($"Error checking registry for SQL instances: {ex.Message}");
        }
    }

    private void LoadDatabases()
    {
        try
        {
            if (cboSqlInstances.SelectedItem == null)
                return;

            var serverAddress = cboSqlInstances.SelectedItem.ToString();
            if (serverAddress == null) return;
            var connString = cbxIntegrated.Checked
                ? GetSqlServerConnectionString(serverAddress, "master")
                : GetSqlServerConnectionString(serverAddress, "master", txtUserDB.Text, txtPassDB.Text);

            using (var conn = new SqlConnection(connString))
            {
                conn.Open();

                // Get the names of all DBs in the database server.
                var query = new SqlCommand(@"select distinct [name] from sysdatabases", conn);
                using (var reader = query.ExecuteReader())
                {
                    cboDatabases.Items.Clear();
                    while (reader.Read())
                        cboDatabases.Items.Add((string)reader[0]);
                    if (cboDatabases.Items.Count > 0)
                        cboDatabases.SelectedIndex = 0;
                } // using
            } // using

            cboDatabases.Enabled = true;
        }
        catch (Exception ex)
        {
            // Log the exception but don't show a message box - just silently fail
            // as this is auto-triggered and showing a message box would be disruptive
            LogMessage($"Error loading databases: {ex.Message}", LogLevel.Error);
            Debug.WriteLine($"Failed to load databases: {ex.Message}");
        }
    }

    private void LogMessage(string message, LogLevel level = LogLevel.Info)
    {
        // Log to the UI tab
        if (rtLog != null && !IsDisposed)
        {
            try
            {
                Invoke(new MethodInvoker(delegate
                {
                    var timestamp = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}]";
                    
                    // Suspend layout to improve performance during updates
                    rtLog.SuspendLayout();
                    
                    // Always append to the end
                    rtLog.SelectionStart = rtLog.TextLength;
                    rtLog.SelectionLength = 0;
                    
                    // Format the timestamp with a distinct color
                    rtLog.SelectionColor = Color.DarkGray;
                    rtLog.AppendText(timestamp + " ");
                    
                    // Format the level prefix with a color based on level
                    string prefix;
                    switch (level)
                    {
                        case LogLevel.Error:
                            rtLog.SelectionColor = Color.Red;
                            prefix = "[ERROR] ";
                            break;
                        case LogLevel.Warning:
                            rtLog.SelectionColor = Color.Orange;
                            prefix = "[WARN] ";
                            break;
                        case LogLevel.Success:
                            rtLog.SelectionColor = Color.Green;
                            prefix = "[SUCCESS] ";
                            break;
                        case LogLevel.Info:
                        default:
                            rtLog.SelectionColor = Color.Black;
                            prefix = "[INFO] ";
                            break;
                    }
                    rtLog.AppendText(prefix);
                    
                    // Format the message text
                    rtLog.SelectionColor = GetMessageColor(message, level);
                    rtLog.AppendText(message + Environment.NewLine);
                    
                    // Ensure caret is at the end so we see the latest text
                    rtLog.SelectionStart = rtLog.TextLength;
                    rtLog.SelectionLength = 0;
                    
                    // Update display immediately
                    rtLog.ScrollToCaret();
                    rtLog.ResumeLayout();
                    rtLog.Update();
                    
                    // Make sure the log tab is properly rendering the text
                    // Force a refresh of the tab control if needed
                    if (!tabPage2.Visible && tabControl1.TabPages.Contains(tabPage2))
                    {
                        tabPage2.Invalidate();
                    }
                }));
            }
            catch
            {
                // Ignore cross-thread operation errors that might occur during shutdown
            }
        }

        // Log to file if enabled
        if (_enableLogging && !string.IsNullOrEmpty(_logFilePath))
        {
            try
            {
                // Create directory if it doesn't exist
                var logDir = Path.GetDirectoryName(_logFilePath);
                if (!string.IsNullOrEmpty(logDir) && !Directory.Exists(logDir))
                {
                    Directory.CreateDirectory(logDir);
                }

                // Format a plain text log entry for the file
                var logLevelText = level.ToString().ToUpper();
                var logEntry = $"[{DateTime.Now:yyyy-MM-dd HH:mm:ss}] [{logLevelText}] {message}";

                // Append to log file
                using StreamWriter writer = new StreamWriter(_logFilePath, true, Encoding.UTF8);
                writer.WriteLine(logEntry);
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error writing to log file: {ex.Message}");
            }
        }
    }

    private Color GetMessageColor(string message, LogLevel level)
    {
        // Base color on the message level
        switch (level)
        {
            case LogLevel.Error:
                return Color.Red;
            case LogLevel.Warning:
                return Color.DarkOrange;
            case LogLevel.Success:
                return Color.Green;
            case LogLevel.Info:
                return Color.DarkOrange;
            default:
                // For info messages, use different colors based on content patterns
                if (message.Contains("SQL Server"))
                    return Color.DarkBlue;
                if (message.Contains("SQLite"))
                    return Color.DarkGreen;
                if (message.Contains("Converting") || message.Contains("conversion"))
                    return Color.Purple;
                if (message.Contains("completed") || message.Contains("finished"))
                    return Color.Green;
                if (message.Contains("failed") || message.Contains("error"))
                    return Color.Red;
                return Color.Black;
        }
    }

    #endregion

    #region Public Enums

    // Define log levels for color-coding and filtering
    public enum LogLevel
    {
        Info,
        Warning,
        Error,
        Success
    }

    #endregion
}