using System.Configuration;

namespace ApplicationForm
{
    public class Settings : ApplicationSettingsBase
    {
        private static Settings defaultInstance = ((Settings)(Synchronized(new Settings())));

        public static Settings Default
        {
            get
            {
                return defaultInstance;
            }
        }

        [UserScopedSetting()]
        [DefaultSettingValue("")]
        public string LastSQLitePath
        {
            get
            {
                return ((string)(this["LastSQLitePath"]));
            }
            set
            {
                this["LastSQLitePath"] = value;
            }
        }

        [UserScopedSetting()]
        [DefaultSettingValue("False")]
        public bool EnableLogging
        {
            get
            {
                return ((bool)(this["EnableLogging"]));
            }
            set
            {
                this["EnableLogging"] = value;
            }
        }

        [UserScopedSetting()]
        [DefaultSettingValue("")]
        public string LogFilePath
        {
            get
            {
                return ((string)(this["LogFilePath"]));
            }
            set
            {
                this["LogFilePath"] = value;
            }
        }
    }
}