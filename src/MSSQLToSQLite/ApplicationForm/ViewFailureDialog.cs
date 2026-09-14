using ClassLibrary;
using System.ComponentModel;

namespace ApplicationForm
{
    public partial class ViewFailureDialog : Form
    {
        private ViewSchema _view = null!; // Use null-forgiving operator to satisfy compiler

        public ViewFailureDialog()
        {
            InitializeComponent();
        }

        // Add DesignerSerializationVisibility attribute to prevent serialization
        [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        [Browsable(false)]  // Add this attribute to avoid designer serialization
        public ViewSchema View
        {
            get { return _view; }
            set
            {
                _view = value;
                Text = "SQL Error: " + _view.ViewName;
                txtSQL.Text = _view.ViewSQL;
            }
        }

        public string ViewSQL
        {
            get { return txtSQL.Text; }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}