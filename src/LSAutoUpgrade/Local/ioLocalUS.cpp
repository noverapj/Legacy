#include "../stdafx.h"
#include "../resource.h"
#include "../HelpFunc.h"
#include ".\iolocalus.h"
#include "../Util/md5.h"
#include <winsock.h>
#include "../Util/ioHttpManager.h"
#include <strsafe.h>
#include "../HttpApp.h"
#include "../HTTPManager.h"
#include "../StringManager/ioHashString.h"
#include "../AutoUpgradeDlg.h"
#include "../ioINILoaderAU.h"

ioLocalUS::ioLocalUS(void)
{
	ZeroMemory( m_szRegKey, sizeof( m_szRegKey ) );
	ZeroMemory( m_szStartURL, sizeof( m_szStartURL ) );
	ZeroMemory( m_szLoginID, sizeof( m_szLoginID ) );
	ZeroMemory( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ) );
	ZeroMemory( m_szServerID, sizeof( m_szServerID ) );
	ZeroMemory( m_szUserType, sizeof( m_szUserType ) );
}

ioLocalUS::~ioLocalUS(void)
{
}

ioLocalManager::LocalType ioLocalUS::GetType()
{
	return ioLocalManager::LCT_US;
}

const char * ioLocalUS::GetTextListFileName()
{
	return "text_us.txt";
}

const char * ioLocalUS::GetMemTextList()
{
	return "|EXE_CAutoUpgradeApp::IsDuplication_1|\r\n\r\n      [Lost Saga] is running.\r\n\r\n      Close [Lost Saga] and try again.\r\n\r\n\r\n      (error code:201)|"
		"|EXE_CAutoUpgradeDlg::OnCopyData_1|Connecting to the Lost Saga server...|"
		"|EXE_CAutoUpgradeDlg::OnCopyData_2|Connecting to UDP...|"
		"|EXE_CAutoUpgradeDlg::OnCopyData_3|Failed connecting to the Lost Saga server.|"
		"|EXE_CAutoUpgradeDlg::OnTimer_1|Lost Saga is running...|"
		"|EXE_CAutoUpgradeDlg::OnTimer_2|Lost Saga was unable to launch for 10 minutes.\r\n(error code:210)|"
		"|EXE_CAutoUpgradeDlg::OnTimer_3|Failed connecting to the Lost Saga server.|"
		"|EXE_CAutoUpgradeDlg::OnTimer_4|Verifying optimization...|"
		"|EXE_CAutoUpgradeDlg::OnTimer_5|Verifying optimization...|"
		"|EXE_CAutoUpgradeDlg::OnTimer_6|Lost Saga will be started automatically.|"
		"|EXE_CAutoUpgradeDlg::OnTimer_7|Lost Saga will be started automatically.|"
		"|EXE_CAutoUpgradeDlg::OnTimer_8|Failed to launch the Lost Saga client.\r\n(error code:226)|"
		"|EXE_CAutoUpgradeDlg::OnTimer_9|Failed to lanunch the Lost Saga client.|"
		"|EXE_CAutoUpgradeDlg::SetWantEndDialog_1|Exiting... Please wait.|"
		"|EXE_Help::CheckRightHDDSpace_1|Updating Lost Saga requires %1 MB or more\r\nfree space in your hard disk.\r\n(error code:207)\r\n\r\nTotal: %2 MB, Free: %3 MB, Required: %4 MB|"
		"|EXE_Help::CheckDownloadedFile_1|Unable to open files for integrity tests.\r\n|"
		"|EXE_Help::CheckDownloadedFile_2|The downloaded file contains webpage errors.\r\nFile name: %1\r\nFileCRC: %2, Size: %3,GetLastError: %4|"
		"|EXE_HTTPManager::_Run_1|Retrieving local patch information...|"
		"|EXE_HTTPManager::_Run_2|The version of the currently installed DirectX is not up-to-date.\r\nPlease install the latest version of DirectX.\r\nMake sure that you have the latest graphics card driver installed.\r\n(error code:211)|"
		"|EXE_HTTPManager::_Run_3|Unable to open %1th iop.\r\nFailed to delete the file.\r\nFile name: %2, GetLastError: %3\r\nPlease run the program again.\r\n(error code:222)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_4|Failed to download the information file.\r\nThe patch server might be busy.\r\nPlease run the program again.\r\n(error code:203)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_5|The data file is corrupted.\r\nThe patch server might be busy.\r\nPlease run the program later.\r\n(error code:215)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_6|Failed to extract the information file package.\r\nMake sure that patch file is correctly synchronized and\r\nrun the program again.\r\n(error code:204)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_7|Failed to parse the information file.\r\n Please run the program again.\r\n(error code:205)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_8|The hard disk space function did not return the value.\r\nThe program will assume that there are enough free space\r\nin the hard disk and proceed accordingly.\r\nPlease secure more hard disk space if the problem persists.\r\n(error code: 223)\r\n\r\nGetLastError:%1|"
		"|EXE_HTTPManager::_Run_9|Unable to download and update the file (failed 100 times).\r\nPlease run the program again.\r\nIf the problem persists, remove Lost Saga from\r\nPrograms and scan the files for viruses.\r\nMake sure that you are connected to the Internet.\r\n(error code: 206)\r\n\r\n|"
		"|EXE_HTTPManager::_Run_10|applying a patch|"
		"|EXE_HTTPManager::CheckPatchPac_1|Check Patch|"
		"|EXE_HTTPManager::UpdatePatchFile_1|Start Pac Patch|"
		"|EXE_HTTPManager::UpdatePatchFile_2|Download Pac[%1/%2]|"
		"|EXE_HTTPManager::UpdatePatchFile_3|Check Pac[%1/%2]|"
		"|EXE_HTTPManager::UpdatePatchFile_4|Update Pac[%1/%2]|"
		"|EXE_HTTPManager::UpdatePatchFile_5|Pac Patch Complate|"
		"|EXE_HTTPManager::DeleteGarbage_1| applying a patch [%1%%/%2%%]|"
		"|EXE_HTTPManager::_RunAdmin_1|Error extracting the package file number %1.\r\n\r\n%2|"
		"|EXE_HTTPManager::_RunAdmin_2|Completed.|"
		"|EXE_HTTPManager::_RunAdmin_3|Error copying folder(s).\r\n\r\n|"
		"|EXE_HTTPManager::_RunOptionCopyFolder_1|Original folder.|"
		"|EXE_HTTPManager::_RunOptionCopyFolder_2|Destination folder.|"
		"|EXE_HTTPManager::_RunOptionCopyFolder_3|Copying folder(s).|"
		"|EXE_HTTPManager::_RunOptionCopyFolder_4|Completed.|"
		"|EXE_HTTPManager::AddPacFile_1|Merging: #%1 file.|"
		"|EXE_HTTPManager::AddPacFile_2|Error merging files: %1.|"
		"|EXE_HTTPManager::AddPacFile_3|Failed to merge files.\r\nFile name: %1\r\nFile name in the package file: %2\r\nPackage file array: %3 , File CRC: %4, GetLastErrorValue: %5, Exception: %6, |"
		"|EXE_HTTPManager::AddPacFile_4|Failed to delete downloaded files while merging files.\r\nFile name: %1\r\nGetLastError:%2, |"
		"|EXE_HTTPManager::CheckPac_1|Error checking package is NULL.|"
		"|EXE_HTTPManager::CheckPac_2|The package parameter for checking is NULL.\r\nGetLastError: %1\r\n|"
		"|EXE_HTTPManager::CheckPac_3|Verifying %1/%2 files: File [%3%%].|"
		"|EXE_HTTPManager::CopyVersionupFiles_1|Processing manual patching: [%1%%].|"
		"|EXE_HTTPManager::CreateAllNewPacFile_1|Failed optimizing #%1 file.\r\n Please run the program again.\r\n(error code: 212)\r\n\r\n%2|"
		"|EXE_HTTPManager::CreateNewPacFile_1|Failed to delete old files to create a new package file.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::CreateNewPacFile_2|Error saving the new package.|"
		"|EXE_HTTPManager::CreateNewPacFile_3|Failed to create and open a new package file.\r\n|"
		"|EXE_HTTPManager::CreateNewPacFile_4|Error: The new package is NULL.|"
		"|EXE_HTTPManager::CreateNewPacFile_5|The iop file parameter for the new package is NULL.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::CreateNewPacFile_6|Error opening a new package.|"
		"|EXE_HTTPManager::CreateNewPacFile_7|Failed to open the iop file for a new package.\r\nFile name: %1\r\nGetLastError: %2,Exception: %3|"
		"|EXE_HTTPManager::CreateNewPacFile_8|New package information error.|"
		"|EXE_HTTPManager::CreateNewPacFile_9|Failed to initialize the file information of the iop file for a new package.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::CreateNewPacFile_10|New package NULL 2 error.|"
		"|EXE_HTTPManager::CreateNewPacFile_11|The new iop parameter value is NULL.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::CreateNewPacFile_12|Error opening the new package 2.|"
		"|EXE_HTTPManager::CreateNewPacFile_13|Failed to open the new iop file.\r\nFile name: %1\r\nGetLastError: %2, File CRC: %3, Exception: %4|"
		"|EXE_HTTPManager::CreateNewPacFile_14|Optimizing:%1/%2 files [%3%%].|"
		"|EXE_HTTPManager::CreateNewPacFile_15|New package information error.|"
		"|EXE_HTTPManager::CreateNewPacFile_16|Failed to read the old iop file information to create a new package .\r\nFile name: %1\r\nGetLastError: %2,Array: %3,Exception: %4|"
		"|EXE_HTTPManager::CreateNewPacFile_17|New package file data error.|"
		"|EXE_HTTPManager::CreateNewPacFile_18|Failed to read the old iop file data to create a new package.\r\nFile name: %1\r\nFile hash: %2,File index: %3,Compressed file size: %4,File size: %5,GetLastError: %6,Array: %7,Exception: %8|"
		"|EXE_HTTPManager::CreateNewPacFile_19|New package file merging error.|"
		"|EXE_HTTPManager::CreateNewPacFile_20|Failed to merge files into a new rs.iop.\r\nFile name: %1\r\n????: %2,File index: %3,Compressed file size: %4,File size: %5,GetLastError: %6,Array: %7,Exception: %8|"
		"|EXE_HTTPManager::CreateNewPacFile_21|New package size error.|"
		"|EXE_HTTPManager::CreateNewPacFile_22|The size of the old iop file for creating a new package is 0.\r\nFile name: %1\r\nFile hash: %2,File index: %3,Compressed file size: %4,File size: %5,GetLastError: %6,Array: %7,Exception: %8|"
		"|EXE_HTTPManager::CreateNewPacFile_23|Error copying the new package.|"
		"|EXE_HTTPManager::CreateNewPacFile_24|Failed to copy the new iop.\r\nFile name: %1\r\nNewFile name: %2\r\nGetLastError: %3|"
		"|EXE_HTTPManager::CreateNewPacFile_25|Error deleting the new package.|"
		"|EXE_HTTPManager::CreateNewPacFile_26|Failed to delete the original file after completing copy.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::DownLoadProcess_1|Downloading #%1 file.|"
		"|EXE_HTTPManager::DownLoadProcess_2|OpenUrl error: %1.|"
		"|EXE_HTTPManager::DownLoadProcess_3|OpenURL failed.\r\nFile name: %1\r\nError: %2|"
		"|EXE_HTTPManager::DownLoadProcess_4|Error OpenUrl NULL: %1.|"
		"|EXE_HTTPManager::DownLoadProcess_5|The CInternetFile value is NULL.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::DownLoadProcess_6|Failed to delete the old files while downloading.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::DownLoadProcess_7|Error opening file: %1.|"
		"|EXE_HTTPManager::DownLoadProcess_8|Failed to open files to save.\r\n|"
		"|EXE_HTTPManager::DownLoadProcess_9|Downloading %1 file[%2%%].|"
		"|EXE_HTTPManager::DownLoadProcess_10|Error writing file: %1.|"
		"|EXE_HTTPManager::DownLoadProcess_11|Failed to write files to save.\r\n|"
		"|EXE_HTTPManager::DownLoadProcess_12|Error receiving file: %1.|"
		"|EXE_HTTPManager::DownLoadProcess_13|Failed to receive files to save.\r\n|"
		"|EXE_HTTPManager::FileCopy_1|Error copying file: %1.|"
		"|EXE_HTTPManager::FileCopy_2|Failed to copy files.\r\nFile name: %1\r\nNew file name: %2\r\nGetLastError: %3, |"
		"|EXE_HTTPManager::GetUpdateFile_1|Verifying downloads.|"
		"|EXE_HTTPManager::GetUpdateFile_2|Verifying updates [%1%%].|"
		"|EXE_HTTPManager::InitPac_1|Failed to assign memory to the package library.\r\n Please run the program again.\r\n(error code: 213)\r\n\r\nArray: %1, GetLastError: %2|"
		"|EXE_HTTPManager::OpenAllBigPacForGarbageSize_1|Failed to open #%1 iop.\r\nFile name: %2, GetLastError: %3\r\nPlease run the program again.\r\n(error code: 225)\r\n\r\n|"
		"|EXE_HTTPManager::ParseDeleteFileList_1|Parsing files to delete.|"
		"|EXE_HTTPManager::ParsePatchFile_1|Failed to open files to parse.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::ParsePatchFile_2|Failed to delete the old files after parsing.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::ProcessCheckPac_1|Problem in client files.\r\n\r\nPress [Yes] to run a complete diagnosis \r\n\r\nand download the client if errors are found.|"
		"|EXE_HTTPManager::ProcessCheckPac_2|Errors found in client files.\r\n\r\nDownload the client again.|"
		"|EXE_HTTPManager::ProcessCheckPac_3|Failed to open #%1 iop.\r\nFailed to delete the file.\r\nFile name: %2, GetLastError: %3\r\nPlease run the program again.\r\n(error code: 224)\r\n\r\n|"
		"|EXE_HTTPManager::ProcessNewPacFile_1|Already optimized.|"
		"|EXE_HTTPManager::ProcessNewPacFile_2|Already optimized.|"
		"|EXE_HTTPManager::Run_1|An exception occurred in the main loop.\r\n Please run the program again.\r\n(error code: 209)\r\n\r\n|"
		"|EXE_HTTPManager::SetTextWaitNewPac_1|Verifying optimization.[%1]|"
		"|EXE_HTTPManager::SetupFull_1|Error extracting #%1 package.\r\n\r\n%2|"
		"|EXE_HTTPManager::SetupFull_2|Failed to delete the installed file version information.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::SetupFull_3|Failed to open the installed version information file.\r\n|"
		"|EXE_HTTPManager::SetupFull_4|Error opening the package file.|"
		"|EXE_HTTPManager::SetupFull_5|Failed to open #%1 iop file after the installation.\r\nFile name: %2\r\nGetLastError: %3, File CRC: %4, Exception: %5, |"
		"|EXE_HTTPManager::StartGame_1|The game is running.|"
		"|EXE_HTTPManager::StartGame_2|The login key has expired.\r\n\r\n         Please log in again\r\n\r\n            for safety.|"
		"|EXE_HTTPManager::StartGame_3|The game will be started automatically.[%1]|"
		"|EXE_HTTPManager::StartGame_4|Press the Start button to start the game.|"
		"|EXE_HTTPManager::StartGame_5|Web Broker Version is low. Please restart the game.|"
		"|EXE_HTTPManager::StartGame_6|Web Broker Error!!|"
		"|EXE_HTTPManager::UnFull_1|Error: Full installation is NULL.|"
		"|EXE_HTTPManager::UnFull_2|The package variable to be used by the installation version is NULL.\r\nGetLastError: %1, |"
		"|EXE_HTTPManager::UnFull_3|Error opening the full installation.|"
		"|EXE_HTTPManager::UnFull_4|Failed to open the full installation.\r\nFile name: %1\r\nGetLastError: %2, File CRC: %3, Exception: %4, |"
		"|EXE_HTTPManager::UnFull_5|Information error: Full installation.|"
		"|EXE_HTTPManager::UnFull_6|Failed to read the installation information.\r\nFile name: %1\r\nGetLastError: %2, File CRC: %3, File array: %4, Exception: %5, |"
		"|EXE_HTTPManager::UnFull_7|Extracting #%1 file[%2%%].|"
		"|EXE_HTTPManager::UnFull_8|Failed to delete the older version files.\r\nFile name: %1\r\nGetLastError: %2, |"
		"|EXE_HTTPManager::UnFull_9|Error extracting the full installation: %1.|"
		"|EXE_HTTPManager::UnFull_10|Failed to extract the full installation file.\r\nFile name: %1\r\nGetLastError: %2, Installation file CRC: %3, Array: %4, Exception: %5, |"
		"|EXE_HTTPManager::UnFull_11|Failed to delete the installed version.\r\nFile name: %1\r\nGetLastError: %2, |"
		"|EXE_HTTPManager::UnPac_1|Error: Full package is NULL.|"
		"|EXE_HTTPManager::UnPac_2|The rs.iop file variable used to extract package files is NULL.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UnPac_3|Error opening the full package.|"
		"|EXE_HTTPManager::UnPac_4|Failed to open the rs.iop file used to extract package files.\r\nFile name: %1\r\nGetLastError: %2,Exception: %3|"
		"|EXE_HTTPManager::UnPac_5|Full package INIT error.|"
		"|EXE_HTTPManager::UnPac_6|Failed to initialize the rs.iop file to extract package files.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UnPac_7|Full package information error.|"
		"|EXE_HTTPManager::UnPac_8|Failed to read the old rs.iop file information to extract package files.\r\nFile name: %1\r\nGetLastError: %2,Array: %3,Exception: %4|"
		"|EXE_HTTPManager::UnPac_9|Extracting IOP: %1/%2 file [%3%%].|"
		"|EXE_HTTPManager::UnPac_10|Error deleting the old file to extract package files.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UnPac_11|Error extracting the full package.|"
		"|EXE_HTTPManager::UnPac_12|Failed to extract files to extract package files.\r\nFile name: %1\r\nFile hash: %2,File index: %3,Compressed file size: %4,GetLastError: %5,Array: %6,Exception: %7|"
		"|EXE_HTTPManager::UnPacPieceFile_1|Extracting #%1 file.|"
		"|EXE_HTTPManager::UnPacPieceFile_2|Failed to delete old files during individual extraction.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UnPacPieceFile_3|Failed to find the files to extract during individual extraction.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UnPacPieceFile_4|Error extracting individual file: %1.|"
		"|EXE_HTTPManager::UnPacPieceFile_5|Failed to extract individual files.\r\nFile name: %1\r\nError return value: %2, File CRC: %3, GetLastError: %4, Exception: %5|"
		"|EXE_HTTPManager::UnPacPieceFile_6|Failed to delete downloaded files during individual extraction.\r\nFile name: %1\r\nGetLastError: %2|"
		"|EXE_HTTPManager::UpdateFileLoop_1|Parsing files to download.|"
		"|EXE_HTTPManager::UpdateFileLoop_2|Retrying due to error: %1/%2.|"
		"|EXE_HTTPManager::UpdateOneFile_1|An error occurred while downloading files.\r\nThe program will retry downloading (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 218)\r\n\r\n|"
		"|EXE_HTTPManager::UpdateOneFile_2|Error: Downloaded file corruption: %1.|"
		"|EXE_HTTPManager::UpdateOneFile_3|The downloaded file is damaged.\r\nThe patch server might be busy.\r\nThe program will retry downloading the file. (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 216)\r\n\r\n|"
		"|EXE_HTTPManager::UpdateOneFile_4|Downloaded file size error.|"
		"|EXE_HTTPManager::UpdateOneFile_5|Invalid file size.\r\nFile name: %1\r\nServer file size: %2, Downloaded file size: %3, Server file CRC: %4, Downloaded file CRC: %5|"
		"|EXE_HTTPManager::UpdateOneFile_6|The size of the downloaded file has been changed.\r\nThe patch information file maybe not up-to-date or\r\ninfected by virus.\r\nIf the problem persists, try the followings:\r\n\r\n1. Delete the [Lost Saga] installation folder.\r\n   For Internet Explorer 7 users, Tools -> Internet options -> General -> Delete -> Delete all-> Select [Delete files and settings stored by add-ons.] -> Yes\r\n   For Internet Explorer 6 users, Tools -> Internet options -> General -> Delete Cookies ->  Delete\r\n                                   Tools -> Internet options -> General -> Delete Files -> Select [Delete all offline content] ->|"
		"|EXE_HTTPManager::UpdateOneFile_7|Sever file size: %1, Downloaded file size: %2, Server file CRC: %3|"
		"|EXE_HTTPManager::UpdateOneFile_8|An error occurred while merging files.\r\nThe program will retry merging. (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 219)\r\n\r\n|"
		"|EXE_HTTPManager::UpdateOneFile_9|Server file size: %1, Downloaded file size: %2, Server file CRC: %3|"
		"|EXE_HTTPManager::UpdateOneFile_10|An error occurred during individual file extraction.\r\nThe program will retry extracting. (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 220)\r\n\r\n|"
		"|EXE_HTTPManager::UpdateOneFile_11|Server file size: %1, Downloaded file size: %2, Server file CRC: %3|"
		"|EXE_HTTPManager::UpdateOneFile_12|An error occurred while copying files.\r\nThe program will retry copying. (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 221)\r\n\r\n|"
		"|EXE_HTTPManager::UpdateOneFile_13|Server file size: %1, Downloaded file size: %2|"
		"|EXE_HTTPManager::UpdateOneFile_14|Failed to install the full version.\r\nThe program will retry installing. (max. 100 times)\r\nClick the X button\r\nto close the program.\r\n(error code: 217)\r\n\r\n|"
		"|EXE_ioErrorDlg::ShowErrorDlg_1|Ver : %1\r\n--- System Requirements ---\r\n%2\r\n\r\n--- Error Information ---\r\n%3|"
		"|EXE_ioLoginDlg::FirstConnectServer_1|Connecting to the %1 server.|"
		"|EXE_ioLoginDlg::OnTimer_1|%1 server not responding, connecting to the %2 server.|"
		"|EXE_ioLoginDlg::OnTimer_2|%1 server not responding, connecting to the %2 server.|"
		"|EXE_ioLoginDlg::SendServer_1|The login request is sent to the %1 server, waiting for response.|"
		"|EXE_ioLoginDlg::SetTextNetworkConnected_1|Connected to the %1 server.|";
}

void ioLocalUS::GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType /*=0*/ )
{
	if( iPasswordType == 0 )
	{
		// password : eE39DkE!%E0
		char szEncPassWord[MAX_PASSWORD]={ -101, 90, 44, 51, 25, 74, 41, -121, 99, 98, 48, 39, 70, -90, 108, 33, 93, 10, 31, 31 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
	else if( iPasswordType == 1 )
	{
		// password : Eg%^io03UT$Cvf921-!$
		char szEncPassWord[MAX_PASSWORD]={ -69, 120, 58, 84, 52, 78, 92, -107, 19, 115, 36, 100, 48, -64, 85, 19, 108, 39, 62, 59 };
		EncryptDecryptData( szPassword, iPasswordSize, szEncPassWord, MAX_PASSWORD, true );
	}
}

DWORD ioLocalUS::GetResourceIDBackBmp()
{
	return IDB_BACK_US;
}

DWORD ioLocalUS::GetResourceIDOptimizeBtnBmp()
{
	return IDB_OPTIMIZE_BTN_US;
}

DWORD ioLocalUS::GetResourceIDStartBtnBmp()
{
	return IDB_START_US;
}

DWORD ioLocalUS::GetResourceIDErrorBackBtnBmp()
{
	return IDB_ERROR_BACK_US;
}

DWORD ioLocalUS::GetResourceIDErrorSolutionBtnBmp()
{
	return IDB_SOLUTIONBTN_US;  
}

DWORD ioLocalUS::GetResourceIDErrorExitBtnBmp()
{
	return IDB_EXITBTN_US;
}

const char * ioLocalUS::GetRegKey()
{
	return "Software\\SP2ClientUS\\";
}

const char * ioLocalUS::GetFontName()
{
	return "Tahoma";
}

void ioLocalUS::CreateShortcuts( const CString &rszRootDir, const CString &rszShortcutName, const CString &rszLoginURL )
{
	Help::CreateShortcutOnDeskTop( rszRootDir, rszShortcutName, rszLoginURL );
	Help::CreateMenuIcon( rszShortcutName );

	CString szClientPath = "lostsaga.exe";
	CString szFullPath;
	Help::SetFullPath( rszRootDir, szClientPath, szFullPath);

	char szVersionName[MAX_PATH*2]="";
	Help::GetFileVesrion((LPCTSTR)szFullPath, szVersionName, sizeof(szVersionName), false );
	Help::CreateUninstallRegInfo( rszRootDir, szVersionName );
}

const char * ioLocalUS::GetErrorSolutionURL()
{
	return "http://support.z8games.com/lostsaga/";
}

bool ioLocalUS::GetNewCmdLine( IN const char *szCmd, OUT char *szNewCmd, IN int iNewCmdSize )
{

#ifdef STEAM_ATTACH
	/*ioINILoaderAU kLoader( "info\\cmd.ini" );
	kLoader.SetTitle( "Cmdline" );*/
	if( strcmp( m_szLoginEncodePW, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "LoginPW is empty." );		
		//kLoader.SaveInt("return",1);
		return false;
	}

	char szCurCmd[4096]="";
	ZeroMemory( szCurCmd, sizeof( szCurCmd ) );
	StringCbCopy( szCurCmd, sizeof( szCurCmd ), szCmd );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

#else
	if( strcmp( m_szLoginID, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "LoginID is empty." );
		return false;
	}

	if( strcmp( m_szLoginEncodePW, "" ) == 0 )
	{
		StringCbCopy( szNewCmd, iNewCmdSize, "LoginPW is empty." );
		return false;
	}

	char szCurCmd[4096]="";
	ZeroMemory( szCurCmd, sizeof( szCurCmd ) );
	StringCbCopy( szCurCmd, sizeof( szCurCmd ), szCmd );

#endif
	enum 
	{ 
		MAX_LOOP           = 5, 
		TYPE_EXPAND_KEY    = 0, 
		TYPE_KEY_VALUE     = 1,
		TYPE_GAMESERVER_ID = 3,
	};

	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szCurCmd, CMD_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_SECTION_TOKEN );

		if( pPos == NULL )
			break;

#ifdef STEAM_ATTACH
		if( i == TYPE_EXPAND_KEY )
		{
			StringCbCat( szNewCmd, iNewCmdSize, " " );  // cmd 라인 빈칸 입력 a.exe aaaa
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
		else if( i == TYPE_KEY_VALUE )
		{
			// 키값은 0으로 쓰레기 값이므로 복사하지 않는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginEncodePW );
		}
		else if( i == TYPE_GAMESERVER_ID )
		{
			// 정상 접속이 되는 서버ID를 넣는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );

			if( pDlg && pDlg->IsGameServerID() )
			{
				std::string szServerID;
				pDlg->GetStrGameServerID(szServerID);
				SetServerID(szServerID.c_str());
				StringCbCat( szNewCmd, iNewCmdSize, m_szServerID );
			}
			else
			{
				StringCbCat( szNewCmd, iNewCmdSize, pPos );
			}
		}
		else 
		{
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
#else
		if( i == TYPE_EXPAND_KEY )
		{
			StringCbCat( szNewCmd, iNewCmdSize, " " );  // cmd 라인 빈칸 입력 a.exe aaaa
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
			StringCbCat( szNewCmd, iNewCmdSize, CMD_EXPAND_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginEncodePW );
		}
		else if( i == TYPE_KEY_VALUE )
		{
			// 키값은 0으로 쓰레기 값이므로 복사하지 않는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szLoginID );
		}
		else if( i == TYPE_GAMESERVER_ID )
		{
			// 정상 접속이 되는 서버ID를 넣는다.
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, m_szServerID ); 
		}
		else 
		{
			StringCbCat( szNewCmd, iNewCmdSize, CMD_SECTION_TOKEN );
			StringCbCat( szNewCmd, iNewCmdSize, pPos );
		}
#endif
	}

	return true;
}

bool ioLocalUS::GetLoginResultPostData( OUT char *szError, IN int iErrorSize, const char *szURL, const char *szID, const char *szPW )
{
	return true;
}

int ioLocalUS::GetLoginKeyLiveTime()
{
	return ( LOGIN_KEY_LIVE_TIME*100 ); // 서버에서 로그인을 하므로 로그인키 생존 시간이 필요 없기때문에 아주길게 설정
}

void ioLocalUS::SetLoginData( const char *szLoginID, const char *szLoginEncodePW )
{
	if( szLoginID == NULL )
		return;
	if( szLoginEncodePW == NULL )
		return;

	StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), szLoginID );
	StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), szLoginEncodePW );
}

void ioLocalUS::SetServerID( const char *szServerID )
{
	if( szServerID == NULL )
		return;

	StringCbCopy( m_szServerID, sizeof( m_szServerID ), szServerID );
}

bool ioLocalUS::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 3,  };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > GetMaxIDSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='-') &&
			(szID[i]!='_') &&
			(szID[i]!='@') &&
			(szID[i]!='.') &&
			(szID[i]!='+') &&
			(szID[i]!='|') ) // 구분자
		{
			return false;
		}
	}
	return true;
}

bool ioLocalUS::IsRightPW( const char *szPW )
{
	enum { MIN_LENGTH = 3,  };

	int iSize = strlen( szPW );
	if ( iSize < MIN_LENGTH || iSize > GetMaxPWSize() )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szPW[i], 'A', 'Z'+1)) &&
			(!COMPARE(szPW[i], 'a', 'z'+1)) &&
			(!COMPARE(szPW[i], '0', '9'+1)) )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalUS::GetConnectFailMent()
{
	return "Cannot connect the server.\r\n\r\n Due to problem with Computer’s Network Program setup \r\n the connection to server failed.(TCP Connection Failed)\r\n(Error Code:227)";
}

const char * ioLocalUS::GetLoginWrongIDMent()
{
	return "Wrong ID";
}

const char * ioLocalUS::GetLoginWrongPWMent()
{
	return "Wrong Password";
}

const char * ioLocalUS::GetServerFullMent()
{
	return "Game Server is full.\r\n(Error Code:228)";
}

const char * ioLocalUS::GetEntryURL()
{
	return "http://lostsaga.z8games.com/landing/";
}

const char * ioLocalUS::GetFindIDURL()
{
	return "http://www.z8games.com/lost_id_password.aspx";
}

const char *ioLocalUS::GetLoginDlgURL()
{
	return "http://images.z8games.com/lsna/launcher/img/launcher_main.jpg";
}

const char * ioLocalUS::GetFindPWURL()
{
	return ""; // facebook
}

void ioLocalUS::SetUserType( const char *szUserType )
{
	StringCbCopy( m_szUserType, sizeof( m_szUserType ), szUserType );
}

void ioLocalUS::FillLoginData( OUT SP2Packet &rkPacket )
{
	rkPacket << m_szUserType;
}


void ioLocalUS::OnFacebookData( const string &rsEncryptData )
{
	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH*2]="";
	if( rsEncryptData.length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), rsEncryptData.c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH*2]="";
	int iEncodeCnt = 0;
	int iEncodPwLength = rsEncryptData.length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = rsEncryptData.at(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH*2 )
				break;
		}
	}

	char szFBIDToken[MAX_PATH*2]="";
	Help::Decode( szEncode, strlen( szEncode ), szFBIDToken, sizeof( szFBIDToken ), szRandomKey, strlen( szRandomKey ) );

	char szID[MAX_PATH]="";
	char szToken[MAX_PATH*2]="";

	enum 
	{ 
		MAX_LOOP           = 2,
		FB_ID_ARRAY        = 0,
		FB_TOKEN_ARRAY     = 1,
	};
	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szFBIDToken, FACEBOOK_DATA_TOKEN );
		else
			pPos = strtok( NULL, FACEBOOK_DATA_TOKEN );

		if( pPos == NULL )
			break;

		if( i == FB_ID_ARRAY )
		{
			StringCbCopy( m_szLoginID, sizeof( m_szLoginID ), pPos );
		}
		else if( i == FB_TOKEN_ARRAY )
		{
			StringCbCopy( m_szLoginEncodePW, sizeof( m_szLoginEncodePW ), pPos );
		}
	}

	HWND hWnd = ::FindWindow( NULL, "LostSaga FB Login" );
	if(hWnd != NULL)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 0;
		cds.cbData = 0;
		cds.lpData = NULL;
		::SendMessage(hWnd, WM_COPYDATA,0,(LPARAM)&cds);
	}

	SetUserType( "FB" );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( pDlg )
		pDlg->SetLoginTimer();
}

#ifdef STEAM_ATTACH
int ioLocalUS::UpdateNRunSteamLauncher( HTTPManager* HttpMgr)
{
	if( 1 < strlen(m_szLoginID) || 1 < strlen(m_szLoginEncodePW) )
		return 0;

	CString strSteamFileURL = "http://cdn.lostsaga.z8games.com/Launcher/LastVersion/"; //ini
	int iStep = STEAM_FILE_CHECK;

	CString strCurrentFolderPath;
	GetWorkingFolderPath(strCurrentFolderPath);
	CString strFileName = "LoginLauncherN.exe";
	if( iStep == STEAM_FILE_CHECK )
	{
		if( !CheckFileInFolder(&strCurrentFolderPath,&strFileName) )
			iStep = STEAM_DOWN;
		else //파일 용량 검사
		{
			CStdioFile file;
			file.Open((strCurrentFolderPath+strFileName), CFile::typeBinary);
			if( file.GetLength() > (1024 * 20)) //정상
				iStep = STEAM_RUN; 
			else
			{
				DeleteFile(strCurrentFolderPath + strFileName); //지우고 다시 받음
				iStep = STEAM_DOWN;
			}
		}
	}
	if( iStep == STEAM_DOWN )
	{
		int iDownCnt = 0;
		DWORD dwFileSize;
		while( 1 )
		{
			if(HttpMgr->DownLoadOneFile( strSteamFileURL+strFileName,0, strCurrentFolderPath + strFileName, dwFileSize ))
			{
				if( dwFileSize < ( 1024 * 20 )) //20kbyte
				{
					DeleteFile(strCurrentFolderPath + strFileName);
					MessageBox(NULL, "Download Size Error SteamLauncher", "DOWNLOAD ERROR", MB_OK);
					return STEAM_FAIL;
				}
				iStep = STEAM_RUN;
				break;
			}
			else
				++iDownCnt;
			if( iDownCnt > 3 )
			{
				MessageBox(NULL, "Download Fail SteamLauncher", "DOWNLOAD ERROR", MB_OK);
				return STEAM_FAIL;
			}
		}
	}
	if( iStep == STEAM_RUN )
	{
		CString strPath = (strCurrentFolderPath+strFileName);
		Help::CreateProcessWithInfo((LPSTR)(LPCSTR)strPath,"");
		return STEAM_RUN;
	}
}

bool ioLocalUS::CheckFileInFolder( CString* const strPath, CString* const strFile )
{
	CFileFind pFind;
	CString strSearchPath;
	strSearchPath = *strPath;
	strSearchPath += '\\';
	strSearchPath += *strFile;
	return pFind.FindFile(strSearchPath);
}

bool ioLocalUS::DownloadLauncher( const CString & strLauncherURL, HTTPManager* HttpMgr )
{
	enum { eFULL_PATH_SIZE = 1024, eDRIVE_PATH_SIZE = 24, eDIR_PATH_SIZE = 512, eFILE_PATH_SIZE = 256, eEXT_PATH_SIZE = 24};

	char szModuleName[1024] = {0,};
	GetModuleFileName(NULL, szModuleName, eFULL_PATH_SIZE);
	LOG.PrintTimeAndLog(0, "GetModuleFileName - %s", szModuleName);

	char szDrive[eDRIVE_PATH_SIZE] = {0,};
	char szDir[eDIR_PATH_SIZE] = {0,};
	_splitpath(szModuleName, szDrive, szDir, NULL, NULL);

	CString strLauncherPath = szDrive;
	strLauncherPath += szDir;

	char * szLauncher = "LoginLauncherN.exe";
	strLauncherPath += szLauncher;

	enum { eMIN_FILE_SIZE = 20 * 1024, eMAX_DOWNLOAD_COUNT = 5, };

	bool bFileValid = false;
	CFileFind fileFind;
	bool bFileExist = fileFind.FindFile(strLauncherPath);
	if(bFileExist)
	{
		fileFind.FindNextFile();
		// 일단 사이즈로 정상 파일인지 체크
		if(fileFind.GetLength() > eMIN_FILE_SIZE)
		{
			bFileValid = true;
		}
	}

	if(!bFileValid)
	{
		if(HttpMgr)
		{
			CString strFullLauncherURL = strLauncherURL;
			strFullLauncherURL += szLauncher;

			DWORD dwFileSize = 0;
			bool bDownload = false;
			for(int downloadCount = 0; downloadCount < eMAX_DOWNLOAD_COUNT; ++downloadCount)
			{
				bDownload = HttpMgr->DownLoadOneFile(strFullLauncherURL, 0, strLauncherPath, dwFileSize);
				if(bDownload)
				{
					//다운실패 간주
					if(dwFileSize < eMIN_FILE_SIZE)
					{
						continue;
					}
					else
					{
						break;
					}
				}
			}
			if(!bDownload)
			{
				MessageBox(NULL, "Launcher Download Fail.", "DOWNLOAD ERROR", MB_OK);
				LOG.PrintTimeAndLog(0, "CheckLoginValidation - Launcher Download Fail");
				return false; //fail down
			}
		}
		else
		{
			MessageBox(NULL, "httpMgr Isn't Exist", "DOWNLOAD ERROR", MB_OK);
			LOG.PrintTimeAndLog(0, "CheckLoginValidation - httpMgr Isn't Exist");
			return false;
		}
	}

	Help::CreateProcessWithInfo(strLauncherPath, "");
	LOG.PrintTimeAndLog(0, "CheckLoginValidation - Start LoginLauncherN Complete");

	return true;
}

bool ioLocalUS::IsLoginDataValidate()
{
	/*ioINILoaderAU kLoader( "info\\cmd.ini" );
	kLoader.SetTitle( "Cmdline" );
	kLoader.SaveString("m_szLoginID",m_szLoginID);
	kLoader.SaveString("m_szLoginEncodePW",m_szLoginEncodePW);*/
	if(!strlen(m_szLoginID) || !strlen(m_szLoginEncodePW) ||
		!strcmp("0", m_szLoginID) || !strcmp("0", m_szLoginEncodePW))
	{
		return false;
	}
	return true;
}


void ioLocalUS::GetWorkingFolderPath( OUT CString& strPath )
{
	char szFullPath[MAX_PATH]= {0,};
	char szDrivePath[MAX_PATH]= {0,};
	char szDirPath[MAX_PATH]= {0,};
	char szFileName[MAX_PATH]= {0,};
	char szExt[MAX_PATH]= {0,};
	GetModuleFileName(NULL, (LPSTR)szFullPath,MAX_PATH);
	::_splitpath(szFullPath,szDrivePath,szDirPath,NULL,NULL);
	strPath = szDrivePath;
	strPath += szDirPath;
}

#endif