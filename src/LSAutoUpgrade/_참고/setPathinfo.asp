<script language="javascript" type="text/javascript">

var AUTOUPGRADE_EXE_CRC													= "3268034361";
var AUTOUPGRADE_INFO_INI_CRC											= "335708905";
var READ_REG_CLIENT_PATH													= "1";
var FULLZIP_VERSION																= "2010,7,15,1";
var GARBAGE_MB																	= "10,20,30,1";
var EXEC_CHECKKEY																= "EDEW3940FVDP4950";
var EXTEND_KEY																	= EXEC_CHECKKEY + "," + GARBAGE_MB;
var SZ_FOLDER_NAME															= "LostSaga";
var PATH_BROKERFILENAME													= "\\lstwwebbroker.exe";
var PATH_SETUP_FOLDER_ROOT												= "C:\\Program Files\\" + SZ_FOLDER_NAME;
var PATH_SETUP_FILENAME_AUTOUPGRADE_EXE	 					= "autoupgrade.exe";
var PATH_SETUP_FILENAME_FULLZIP										= "full.zip";
var PATH_SETUP_FILENAME_AUTOUPGRADE_INI						= "autoupgrade_info.ini"
var PATH_SETUP_FOLDER_INI													= PATH_SETUP_FOLDER_ROOT + "\\Info";
var PATH_SETUP_FOLDER_FULLZIP											= PATH_SETUP_FOLDER_ROOT + "\\" + PATH_SETUP_FILENAME_FULLZIP;
var PATH_SETUP_FOLDER_AUTOUPGRADE_EXE						= PATH_SETUP_FOLDER_ROOT + "\\" + PATH_SETUP_FILENAME_AUTOUPGRADE_EXE;
var PATH_SETUP_FOLDER_AUTOUPGRADE_INI							= ""
var HTTP_AUTOUPGRADE_EXE												= "http://patch.lostsaga.com.tw/lostsaga/AutoUpgrade/autoupgrade.exe";
var HTTP_AUTOUPGRADE_INI													= "http://patch.lostsaga.com.tw/lostsaga/AutoUpgrade/" + PATH_SETUP_FILENAME_AUTOUPGRADE_INI;
var LocalType																			= 521837584
var USKey																				= 0
var addparam																		= "0,0,1," + LocalType + "," + USKey;		//cptype, extendparam, limitedcert, localtype, uskey

FULLZIP_VERSION																	= FULLZIP_VERSION + "?" + READ_REG_CLIENT_PATH;
EXTEND_KEY																			= EXTEND_KEY + "," + PATH_SETUP_FILENAME_AUTOUPGRADE_INI + "," + addparam;
PATH_SETUP_FOLDER_AUTOUPGRADE_INI								= PATH_SETUP_FOLDER_INI + "\\" + PATH_SETUP_FILENAME_AUTOUPGRADE_INI;
</script>
