using ClassLibrary;
using System.Diagnostics;
using System.Reflection;

namespace ApplicationForm
{
    /// <summary>
    /// Helper to fix the SqlServerToSqLite class behavior with dialog display
    /// </summary>
    public static class SqlServerToSqLiteFixer
    {
        /// <summary>
        /// Forces the SqlServerToSqLite class to show dialogs by using reflection if necessary
        /// </summary>
        public static void EnsureDialogsWillShow()
        {
            try
            {
                // Try to access the IsSilent property via reflection to make sure it's explicitly set to false
                var type = typeof(SqlServerToSqLite);
                var field = type.GetField("IsSilent", BindingFlags.Public | BindingFlags.Static);
                
                if (field != null)
                {
                    // Check the current value
                    bool currentValue = (bool)field.GetValue(null);
                    Debug.WriteLine($"Current IsSilent value: {currentValue}");
                    
                    // Set it to false to ensure dialogs are shown
                    field.SetValue(null, false);
                    
                    // Verify it worked
                    bool newValue = (bool)field.GetValue(null);
                    Debug.WriteLine($"New IsSilent value: {newValue}");
                }
                else
                {
                    Debug.WriteLine("Could not find IsSilent field");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Failed to force dialogs to show: {ex.Message}");
            }
        }
    }
}