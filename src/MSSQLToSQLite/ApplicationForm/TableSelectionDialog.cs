using ClassLibrary;

namespace ApplicationForm
{
    /// <summary>
    /// The dialog allows the user to select which tables to include in the 
    /// conversion process.
    /// </summary>
    public partial class TableSelectionDialog : Form
    {
        #region Constructors
        public TableSelectionDialog()
        {
            InitializeComponent();
        }
        #endregion

        #region Public Properties
        /// <summary>
        /// Returns the list of included table schema objects.
        /// </summary>
        public List<TableSchema> IncludedTables
        {
            get
            {
                List<TableSchema> res = new List<TableSchema>();
                foreach (DataGridViewRow row in grdTables.Rows)
                {
                    bool include = (bool)row.Cells[0].Value;
                    if (include)
                        res.Add((TableSchema)row.Tag);
                } // foreach

                return res;
            }
        }
        #endregion

        #region Public Methods
        /// <summary>
        /// Opens the table selection dialog and uses the specified schema list in order
        /// to update the tables grid.
        /// </summary>
        /// <param name="schema">The DB schema to display in the grid</param>
        /// <param name="owner">The owner form</param>
        /// <returns>dialog result according to user decision.</returns>
        public DialogResult ShowTables(List<TableSchema> schema, IWin32Window owner)
        {
            UpdateGuiFromSchema(schema);
            this.Text = $"Select Tables To Convert ({schema.Count} tables found)";
            return this.ShowDialog(owner);
        }
        #endregion

        #region Event Handlers
        private void btnOK_Click(object sender, EventArgs e)
        {
            // Check if any tables are selected
            bool anyTableSelected = false;
            foreach (DataGridViewRow row in grdTables.Rows)
            {
                bool include = (bool)row.Cells[0].Value;
                if (include)
                {
                    anyTableSelected = true;
                    break;
                }
            }

            // Show warning if no tables are selected
            if (!anyTableSelected)
            {
                DialogResult result = MessageBox.Show(
                    "You haven't selected any tables to convert. Do you want to select all tables?",
                    "No Tables Selected",
                    MessageBoxButtons.YesNoCancel,
                    MessageBoxIcon.Warning);
                
                if (result == DialogResult.Yes)
                {
                    // Select all tables
                    foreach (DataGridViewRow row in grdTables.Rows)
                    {
                        row.Cells[0].Value = true;
                    }
                    DialogResult = DialogResult.OK;
                }
                else if (result == DialogResult.No)
                {
                    // Continue with no tables selected
                    DialogResult = DialogResult.OK;
                }
                // If Cancel, do nothing and keep the dialog open
            }
            else
            {
                DialogResult = DialogResult.OK;
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            // Ask user if they want to continue without selecting tables
            DialogResult result = MessageBox.Show(
                "If you cancel, all tables will be included by default. Continue?",
                "Include All Tables",
                MessageBoxButtons.YesNo,
                MessageBoxIcon.Question);
                
            if (result == DialogResult.Yes)
            {
                DialogResult = DialogResult.Cancel;
            }
            // If No, do nothing and keep the dialog open
        }

        private void btnDeselectAll_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in grdTables.Rows)
            {
                // Uncheck the [V] for this row.
                row.Cells[0].Value = false;
            } // foreach
        }

        private void btnSelectAll_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in grdTables.Rows)
            {
                // Check the [V] for this row.
                row.Cells[0].Value = true;
            } // foreach
        }
        #endregion

        #region Private Methods
        private void UpdateGuiFromSchema(List<TableSchema> schema)
        {
            grdTables.Rows.Clear();
            foreach (TableSchema table in schema)
            {
                // By default all tables are selected
                grdTables.Rows.Add(true, table.TableName);
                grdTables.Rows[grdTables.Rows.Count - 1].Tag = table;
            } // foreach
        }
        #endregion
    }
}