#include "StdAfx.h"
#include "AutoUpgradeDlg.h"
#include "SystemInfo.h"
#include "Crc32Static.h"
#include "ioINILoaderAU.h"
#include "CreateShortcut.h"
#include "StringManager/safesprintf.h"
#include "StringManager/ioStringManager.h"
#include <strsafe.h>
#include ".\helpfunc.h"
#include "mmsystem.h"
#include "Local\ioLocalParent.h"
#include "NetWork\ioClientSocket.h"

#ifdef MAC_ADDRESS 
#include <iphlpapi.h>
#endif
//#ifdef STEAM_ATTACH
#include <tlhelp32.h> 
//#endif

//External IP
#include <wininet.h>
#pragma comment(lib, "wininet")


// CheckDXVersion.cpp for IsRightDXVersion()
//HRESULT CheckDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );

namespace Help
{
static bool sg_bPieceFilePermit = false;

bool RegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault,LPTSTR lpRet, DWORD nSize) 
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)!=ERROR_SUCCESS) 
	{
		LOG.PrintTimeAndLog( 0, "Error RegReadString(%s)",lpKey);
		return false;
	}

	DWORD dwSize = nSize;
	if (RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)lpRet, &dwSize)!=ERROR_SUCCESS) 
	{
		StringCbCopy(lpRet, nSize, lpDefault);
		return false;
	}

	RegCloseKey(key);
	return true;
}

bool RegWriteString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)!=ERROR_SUCCESS) 
		return false;

	if (RegSetValueEx(key, lpValue,0,REG_SZ,(LPBYTE)lpData,strlen(lpData)+1)!=ERROR_SUCCESS) 
		return false;

	RegCloseKey(key);
	return true;
}

int RegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, int iDefault )
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_READ,NULL,&key,&dwDisp)!=ERROR_SUCCESS) 
	{
		LOG.PrintTimeAndLog( 0, "Error RegReadInt");
		return 0;
	}

	int iResult;
	DWORD dwSize = sizeof(int);
	if( RegQueryValueEx(key, lpValue, 0, NULL,(LPBYTE)&iResult, &dwSize)!=ERROR_SUCCESS )
	{
		iResult = iDefault;
	}

	RegCloseKey(key);

	return iResult;
}

bool RegWriteInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, int iData)
{
	HKEY key;
	DWORD dwDisp;
	if (RegCreateKeyEx(hKey, lpKey,0,NULL,REG_OPTION_NON_VOLATILE, KEY_WRITE,NULL,&key,&dwDisp)!=ERROR_SUCCESS) 
		return false;

	if (RegSetValueEx(key, lpValue,0,REG_DWORD,(LPBYTE)&iData,sizeof(int))!=ERROR_SUCCESS) 
		return false;

	RegCloseKey(key);
	return true;
}

void SetUpWindowsFirewall( const char *szPath, const WCHAR *szName )
{
	HRESULT hrComInit = CoInitializeEx(	0,COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE	);
	if( hrComInit != RPC_E_CHANGED_MODE && FAILED(hrComInit) )
		return; 

	WCHAR strGameExeFullPath[MAX_PATH*2];			
	ZeroMemory(strGameExeFullPath, sizeof(strGameExeFullPath));

	if( MultiByteToWideChar(CP_ACP, 0, szPath, -1 , strGameExeFullPath, MAX_PATH*2) == 0)
		 return;

	if( !FirewallWrapper::CanHostMultiplayer( strGameExeFullPath ) )
		FirewallWrapper::OnInstallApplication( strGameExeFullPath, szName );	

	// Uninitialize COM.
	if( SUCCEEDED(hrComInit) )
		CoUninitialize();

	LOG.PrintTimeAndLog(0, "Complete firewall.");
}

bool MoveFileRepeat(const char* szExistPath, const char* szNewPath)
{
	for(int i = 0 ; i < MAX_FILE_CONTROL_LOOP ; i++) 
	{
		SetFileAttributes(szNewPath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(szNewPath);

		if( MoveFile(szExistPath, szNewPath) != 0) // 성공 
			return true;
		Sleep(1); 
	}
	return false;
}

bool CopyFileRepeat( const char* szExistPath, const char* szNewPath )
{
	for (int i = 0; i < MAX_FILE_CONTROL_LOOP ; i++)
	{
		SetFileAttributes( szNewPath, FILE_ATTRIBUTE_NORMAL );
		::DeleteFile( szNewPath );

		if( CopyFile( szExistPath, szNewPath, true) != 0 ) //성공
			return true;
		Sleep(1);
	}
	return false;
}

bool DeleteFileRepeat(const char *szPath) 
{
	CString strFilename = szPath;
	strFilename = strFilename.Right(strFilename.GetLength() - strFilename.ReverseFind('\\')-1);

	for(int i = 0 ; i < MAX_FILE_CONTROL_LOOP ; i++) 
	{
		DWORD dwReturn = GetFileAttributes(szPath);
		if( dwReturn == FILE_ATTRIBUTE_DIRECTORY)
			return true;
		if( dwReturn == INVALID_FILE_ATTRIBUTES) // 파일 없다. 
			return true;
		if(strFilename == "full.zip") //현재는 full.zip파일만 이어받음
			return true;
		if(DeleteFile(szPath) != 0) 
			return true;

		SetFileAttributes( szPath, FILE_ATTRIBUTE_ARCHIVE ); // 보통 속성으로 변경 
		Sleep(1);
	}
	return false;
}

const char *GetAnnounceFileName(const CString &rPath)
{
	enum { MAX_ANNOUNCE_LENGTH = 20, };
	static char szFileName[MAX_PATH]="";
	memset( szFileName, 0, sizeof(szFileName) );
	_splitpath((LPCTSTR)rPath, NULL, NULL, szFileName, NULL);
	szFileName[MAX_ANNOUNCE_LENGTH]=NULL;

	return szFileName;
}

// bool IsRightDXVersion()
// {
// 	DWORD dwVersion;
// 	char  szVersion[MAX_PATH]="";
// 
// 	HRESULT hr = CheckDXVersion( &dwVersion, szVersion, sizeof(szVersion) );
// 	if( FAILED(hr) )
// 	{
// 		LOG.PrintTimeAndLog(0, "IsRightDXVersion fail - 1.");
// 		return false;
// 	}
// 
// 	if( dwVersion < 0x00090003 )	// 9.0c 
// 	{
// 		LOG.PrintTimeAndLog(0, "IsRightDXVersion fail - 2(%s).", szVersion);
// 		return false;
// 	}
// 
// 	LOG.PrintTimeAndLog(0, "Complete DX version.");
// 	return true;
// }

ReturnValue CheckRightHDDSpace( DWORD dwFullzipSize , OUT CString &rszErrorMsg )
{
	ULONGLONG FullZipSizeX3 = dwFullzipSize * 3;

	ULARGE_INTEGER availDisk;
	ULARGE_INTEGER total;

	if(!GetDiskFreeSpaceEx(NULL, &availDisk, &total, NULL))
		return RV_ANNOUNCE;

	if( FullZipSizeX3 >= availDisk.QuadPart)
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf(  szTempMsg, sizeof( szTempMsg ), STR(1)
			                ,(int) ( ( FullZipSizeX3 - availDisk.QuadPart ) / MEGA )
			                ,(int) (availDisk.QuadPart/MEGA)
			                ,(int) ( ( availDisk.QuadPart - FullZipSizeX3 ) / MEGA )
			                ,(int) (FullZipSizeX3/MEGA) );
		rszErrorMsg = szTempMsg;
		return RV_ERROR;
	}	

	LOG.PrintTimeAndLog(0, "Complete HDD space.");
	return RV_OK;
}

bool GetFileVesrion( IN const char* szFileName, OUT char* szVersionName, IN int iSize , IN bool isProductVerion /*= false*/ )
{
	if(iSize < MAX_PATH) return false;

	DWORD dwArg; 
	DWORD dwInfoSize = GetFileVersionInfoSize(szFileName, &dwArg); 

	if(0 == dwInfoSize) 
		return false;

	BYTE* lpBuff = new BYTE[dwInfoSize]; 

	if(!lpBuff)
		return false;

	if(0 == GetFileVersionInfo(szFileName, 0, dwInfoSize, lpBuff)) 
	{
		delete [] lpBuff; 
		return false;
	}

	VS_FIXEDFILEINFO *vInfo; 
	UINT uInfoSize; 

	if(0 == VerQueryValue(lpBuff, TEXT("\\"), (LPVOID*)&vInfo, &uInfoSize)) 
	{
		delete [] lpBuff; 
		return false;
	}

	if(0 == uInfoSize) 
	{
		delete [] lpBuff; 
		return false; 
	} 

	if(isProductVerion == false)
	{
		StringCbPrintf(szVersionName, iSize, "%d,%d,%d,%d",HIWORD(vInfo->dwFileVersionMS), LOWORD(vInfo->dwFileVersionMS) 
			,HIWORD(vInfo->dwFileVersionLS) , LOWORD(vInfo->dwFileVersionLS) );
	}
	else
	{
		StringCbPrintf(szVersionName, iSize, "%d,%d,%d,%d",HIWORD(vInfo->dwProductVersionMS), LOWORD(vInfo->dwProductVersionMS) 
			,HIWORD(vInfo->dwProductVersionLS) , LOWORD(vInfo->dwProductVersionLS) );
	}
	delete [] lpBuff; 
	return true;
}

void SetCreateDirectoryByFullPath( const char *szDir )
{
	if(szDir == NULL) return;

	const char *pDir = szDir;
	char createDir[MAX_PATH*2] = "";
	int len = 0;
	while (*pDir != '\0') 
	{
		if( *pDir=='/' || 
			*pDir=='\\' )
		{
			CreateDirectory(createDir,NULL);			
		}
		if( len >= (MAX_PATH*2) )
			break;
		createDir[len++] = *pDir;
		pDir++;
	}
	CreateDirectory(createDir,NULL);
}

void SetCreateDirectory( CString szRootDir, CString szShortDir )
{
	if(szShortDir.IsEmpty()) return;

	const TCHAR *pShortDir = (LPCTSTR)szShortDir;

	szRootDir+="\\";
	TCHAR createDir[MAX_PATH] = "";
	_tcscpy(createDir,szRootDir);
	int len = _tcslen(createDir);

	while (*pShortDir != _T('\0')) 
	{
		if( *pShortDir==_T('/') || 
			*pShortDir==_T('\\') )
		{
			CreateDirectory(createDir,NULL);			
		}
		if(len>=MAX_PATH)
			break;
		createDir[len++] = *pShortDir;
		pShortDir++;
	}
	CreateDirectory(createDir,NULL);
}

bool CheckDownloadedFile( const CString &rszDownloadedFullPath , OUT CString &rszErrorMsg )
{
	enum { MAX_CHECK_LENGTH = 21, };
	char szFileRead[MAX_CHECK_LENGTH+1]="";
	CFileException ex;
	CFile file;
	if( !file.Open((LPCTSTR)rszDownloadedFullPath, CFile::modeRead | CFile::shareDenyNone, &ex) )
	{
		rszErrorMsg = STR(1);
		rszErrorMsg += rszDownloadedFullPath;
		rszErrorMsg += "\r\n";
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		rszErrorMsg += szError;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) rszErrorMsg );
		return false;
	}

	int iSize = (int) file.GetLength();
	if( iSize > MAX_CHECK_LENGTH )
		file.Read(szFileRead, MAX_CHECK_LENGTH);
	else
		file.Read(szFileRead, iSize);	
	file.Close();

	CString szWebPage = "<!DOCTYPE HTML PUBLIC";
	// 다운 받은 파일이 웹 페이지라면 
	if(szWebPage.CompareNoCase(szFileRead) == 0)
	{
		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),(LPCTSTR)rszDownloadedFullPath, dwCRC, iSize,GetLastError() );
		rszErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)rszErrorMsg );
		return false;
	}

	LOG.PrintTimeAndLog(0, "Complete check down file.");
	return true;
}

void SetFullPath( IN const CString &szRootDir, IN const CString &szShortPath , OUT CString &szFullPath )
{
	szFullPath = szRootDir;
	szFullPath += "\\";
	szFullPath += szShortPath;
	szFullPath.Replace('/', '\\');
}

void CreateShortcutOnDeskTop( const CString &szRootDir, const CString &szShortcutName, const CString &szURL )
{
	// 바탕화면에 웹페이지 아이콘 생성
	char szDesktopPath[MAX_PATH*2]="";
	SHGetSpecialFolderPath( NULL, szDesktopPath, CSIDL_DESKTOP, FALSE );

	CString szInterShortcutPath = szDesktopPath;
	szInterShortcutPath += szShortcutName;

	// 삭제
	if( !Help::DeleteFileRepeat( (LPCTSTR) szInterShortcutPath) )
	{
		LOG.PrintTimeAndLog(0, "Error - Shortcut - DeleteFileRepeat.");
		return;
	}

	// 아이콘 이름
	CString szIconImagePath = szRootDir;
	szIconImagePath += "\\autoupgrade.exe";

	// 인터넷 바로가기 제작
	CString szShortcutData;
	szShortcutData += "[InternetShortcut]\r\n";
	szShortcutData += "IconFile=";
	szShortcutData += szIconImagePath;
	szShortcutData += "\r\n";
	szShortcutData += "URL=";
	szShortcutData += szURL;
	szShortcutData += "\r\n";
	szShortcutData += "IconIndex=0";

	CFile file;
	if(!file.Open((LPCTSTR)szInterShortcutPath,CFile::modeCreate | CFile::modeWrite))
	{
		LOG.PrintTimeAndLog(0, "Error - Shortcut - Open.");
		return ;	
	}
	file.Write((LPCSTR)szShortcutData,szShortcutData.GetLength());
	file.Close();  
}

void CreateMenuIcon( const CString &a_szShortcutName )
{
	if( !AfxGetApp() )
		return;
	if( !AfxGetApp()->GetMainWnd() )
		return;
	CAutoUpgradeDlg *pDlg = ( CAutoUpgradeDlg* ) AfxGetApp()->GetMainWnd();
	if( !pDlg )
		return;

	char szMenuPath[MAX_PATH*2]="";
	SHGetSpecialFolderPath ( AfxGetApp()->GetMainWnd()->GetSafeHwnd(), szMenuPath, CSIDL_PROGRAMS, FALSE);

	// 폴더
	CString szINIPath = "info\\";
	szINIPath += pDlg->GetININame();
	ioINILoaderAU kIniLoder( (LPCTSTR)szINIPath );
	kIniLoder.SetTitle("autoupgrade_info");
	char szFolderName[MAX_PATH]="";
	kIniLoder.LoadString("program_menu_folder_name", "", szFolderName, sizeof(szFolderName) );

	char szLosaMenuPath[MAX_PATH*2]="";
	StringCbPrintf(szLosaMenuPath,sizeof(szLosaMenuPath), "%s\\%s", szMenuPath,szFolderName );

	CreateDirectory( szLosaMenuPath, NULL );

	// NHN 요청 
	char szSecondFolderName[MAX_PATH]="";
	kIniLoder.LoadString("program_menu_second_folder_name", "", szSecondFolderName, sizeof(szSecondFolderName) );
	if( strcmp( szSecondFolderName, "") != 0 )
	{
		StringCbCat( szLosaMenuPath, sizeof( szLosaMenuPath ), "\\");
		StringCbCat( szLosaMenuPath, sizeof( szLosaMenuPath ), szSecondFolderName );
		CreateDirectory( szLosaMenuPath, NULL );
	}

	// 웹바로가기
	char szDesktopPath[MAX_PATH*2]="";
	SHGetSpecialFolderPath( AfxGetApp()->GetMainWnd()->GetSafeHwnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );
	CString szInterShortcutPath = szDesktopPath;
	szInterShortcutPath += a_szShortcutName;

	char szWebMenuName[MAX_PATH]="";
	kIniLoder.LoadString("program_menu_web_name", "", szWebMenuName, sizeof(szWebMenuName) );

	CString szMenuShortcutPath = szLosaMenuPath;
	szMenuShortcutPath += "\\";
	szMenuShortcutPath += szWebMenuName;

	if(!Help::DeleteFileRepeat( (LPCTSTR) szMenuShortcutPath) )
	{
		LOG.PrintTimeAndLog(0, "Error - CreateMenuIcon - DeleteFileRepeat");
		return;
	}

	SetFileAttributes(szInterShortcutPath,FILE_ATTRIBUTE_NORMAL);
	CopyFile( szInterShortcutPath, szMenuShortcutPath, FALSE );

	// NHN 요청
	if( strcmp( szSecondFolderName, "") != 0 )
	{
		// 제거 바로가기
		char szUninstallMenuName[MAX_PATH]="";
		kIniLoder.LoadString("program_menu_uninstall_name", "", szUninstallMenuName, sizeof(szUninstallMenuName) );

		CString szUninstallPath = szLosaMenuPath;
		szUninstallPath += "\\";
		szUninstallPath += szUninstallMenuName;

		CString szIconImagePath = pDlg->GetRootDir();
		szIconImagePath += "\\lsuninstall.exe";

		CreateShortcut(szIconImagePath, szUninstallPath);
	}
}

void CreateDefaultSettingINI()
{
	char fName[MAX_PATH]="";
	StringCbCopy( fName, sizeof(fName), "Save\\default\\setting.ini" );
	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		CreateDirectory("Save",NULL);
		ZeroMemory(fName, sizeof(fName));
		StringCbCopy( fName, sizeof(fName), "Save\\default" );
		CreateDirectory(fName,NULL);
		ZeroMemory(fName, sizeof(fName));
		StringCbCopy( fName, sizeof(fName), "Save\\default\\setting.ini" );

		fp = fopen( fName, "w" );
		if( !fp )
			return;
	}
	fclose( fp );
}

DWORD GetFileSize( const char *szFileName )
{
	DWORD iFileSize = 0;

	CFile file;
	if(file.Open( szFileName,CFile::modeRead | CFile::shareDenyNone))
	{
		iFileSize = (DWORD) file.GetLength();	
		file.Close();
	}	

	return iFileSize;
}

void CreateUninstallRegInfo( const CString &a_szRootDir , const char *a_szClientVersion )
{
	if( !AfxGetApp() )
		return;
	if( !AfxGetApp()->GetMainWnd() )
		return;
	CAutoUpgradeDlg *pDlg = ( CAutoUpgradeDlg* ) AfxGetApp()->GetMainWnd();
	if( !pDlg )
		return;

	CString szINIPath = "info\\";
	szINIPath += pDlg->GetININame();
	ioINILoaderAU kIniLoder( (LPCTSTR)szINIPath );
	kIniLoder.SetTitle("autoupgrade_info");
	char szFolderName[MAX_PATH]="";
	kIniLoder.LoadString("program_menu_folder_name", "", szFolderName, sizeof(szFolderName) );
	char szLoginURL[MAX_PATH]="";
	kIniLoder.LoadString("login_url", "", szLoginURL, sizeof(szLoginURL) );

	// NHN 요청 
	char szSecondFolderName[MAX_PATH]="";
	kIniLoder.LoadString("program_menu_second_folder_name", "", szSecondFolderName, sizeof(szSecondFolderName) );
	if( strcmp( szSecondFolderName, "") != 0 )
	{
		StringCbCat( szFolderName, sizeof( szFolderName ), szSecondFolderName );
	}

	// 폴더에 따른 유니크값 생성
	DWORD dwCRC32 = 0;
	CString szLowerPath = a_szRootDir;
	szLowerPath.MakeLower();
	CCrc32Static::StringCrc32( (LPCTSTR) szLowerPath, dwCRC32 );
	CString szKey = REG_KEY_UNINSTALL;
	szKey.Format( "%s_%x", REG_KEY_UNINSTALL,  dwCRC32 );

	CString szUninstallerPath = a_szRootDir;
	szUninstallerPath += "\\lsuninstall.exe";

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "DisplayIcon", (LPCTSTR)szUninstallerPath ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 1 : %s", (LPCTSTR)szUninstallerPath );
	}

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "DisplayName", szFolderName ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 2 " );
	}

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "DisplayVersion", a_szClientVersion ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 3 : %s", a_szClientVersion  );
	}

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "Publisher", "IO Entertainment Co., Ltd." ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 4");
	}

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "UninstallString", (LPCTSTR)szUninstallerPath ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 5 : %s", (LPCTSTR)szUninstallerPath  );
	}

	if( !RegWriteString( REG_SHLM, (LPCTSTR)szKey, "URLInfoAbout", szLoginURL ) )
	{
		LOG.PrintTimeAndLog( 0, "Error - CreateUninstallInfo 6" );
	}
}

void ExcuteWeb(const char *szURL)
{
	LOG.PrintTimeAndLog( 0, " - ExcuteWeb Function - " );
	PROCESS_INFORMATION process_info;
	ZeroMemory( &process_info, sizeof(process_info) );	
	STARTUPINFO si;
	ZeroMemory( &si, sizeof(si) );				
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = 0;
	char ProcessName[1024]="";
#if defined(SRC_EU) || defined(SRC_TH) //eu일 경우 브라우저 변경
	LOG.PrintTimeAndLog( 0, "ExcuteWeb (Chrome)!!" );
	RegReadString(HKEY_CURRENT_USER, "SOFTWARE\\Classes\\http\\Shell\\open\\command","","",ProcessName,sizeof(ProcessName));
	if(ProcessName[0] == 0)
		Help::RegReadString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Classes\\http\\Shell\\open\\command","","",ProcessName,sizeof(ProcessName));
	int iCnt = 0;
	for(int i = 0 ; i < 1024 ; ++i) //경로만 잘라내기
	{
		if(ProcessName[i] == '"')
		{
			ProcessName[i] = '\0';
			++iCnt;
		}
		if(iCnt != 0)
			ProcessName[i-1] = ProcessName[i];
		if(iCnt > 1) break;
	}
#else
	LOG.PrintTimeAndLog( 0, "ExcuteWeb (IE)!!" );
	Help::RegReadString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE","","",ProcessName,sizeof(ProcessName));
#endif

	char ExecProcess[1024]="";
	StringCbCopy(ExecProcess,sizeof(ExecProcess), ProcessName);
	StringCbCat(ExecProcess,sizeof(ExecProcess), " ");
	StringCbCat(ExecProcess,sizeof(ExecProcess), szURL);  //검색내용

	if(CreateProcess(NULL,ExecProcess,NULL,NULL,FALSE,0,NULL,NULL,&si,&process_info))
		WaitForInputIdle(process_info.hProcess,10000);
}

void SetPieceFilePermit( bool bPermit )
{
	sg_bPieceFilePermit = bPermit;
}

bool IsPieceFilePermit()
{
	return sg_bPieceFilePermit;
}

void GetRandomKey( OUT char *szRandomKey, IN int iRandomKeySize )
{
	char szRandomValue[MAX_PATH];
	ZeroMemory( szRandomValue, sizeof( szRandomValue ) );
	StringCbPrintf( szRandomValue, sizeof(szRandomValue), "%d%d%d", rand(), timeGetTime(), GetCurrentProcessId() );
	enum { MAX_SIZE = 20, };
	for (int i = 0; i < MAX_SIZE ; i++)
	{
		if( szRandomValue[i] == NULL )
			szRandomValue[i] = rand()%256;
	}
	szRandomValue[MAX_SIZE] = NULL;

	char szMyIP[MAX_PATH]="";
	GetMyIP( szMyIP, sizeof( szMyIP ) );
	char szResult[MAX_PATH];
	ZeroMemory( szResult, sizeof( szResult ) );
	EncryptDecryptData( szResult, sizeof( szResult ), szRandomValue, strlen( szRandomValue ), szMyIP, strlen( szMyIP ) );

	// char를 16진수로 변경
	char szHexRandomValue[MAX_PATH];
	ZeroMemory( szHexRandomValue, sizeof( szHexRandomValue ) );
	int iSize = strlen( szResult );
	for(int i = 0; i < iSize; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", (BYTE)szResult[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat(szHexRandomValue, sizeof( szHexRandomValue ),  szTempHex);
	}
	//

	for (int i = 0; i < MAX_SIZE ; i++)
	{
		if( szHexRandomValue[i] == NULL )
			szHexRandomValue[i] = '0';
	}
	szHexRandomValue[MAX_SIZE] = NULL;
	StringCbCopy( szRandomKey, iRandomKeySize, szHexRandomValue );
}

void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize )
{
	enum { MAX_KEY = 30, MAX_ARRAY = 4, };
	BYTE byKey[MAX_ARRAY][MAX_KEY]={ 1,56,211,49,67,190,123,231,34,6,8,9,56,23,90,8,124,126,137,59,34,23,90,200,201,202,39,98,96,21,
	                                 3,56,211,79,67,191,123,232,34,6,1,9,56,23,91,18,129,126,137,200,34,23,90,202,201,202,39,99,96,22, 
	                                 99,36,111,179,167,91,113,132,39,16,11,19,156,123,191,218,29,26,37,201,32,13,91,222,101,201,139,199,6,21, 
	                                 100,36,11,19,16,9,113,132,39,16,111,19,156,123,191,218,29,26,137,201,32,13,91,222,101,21,13,19,61,21, };

	int iArray = 0;

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA || ioLocalManager::GetLocalType() == ioLocalManager::LCT_PHILIPPINE )
		iArray = 0;
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
		iArray = 1;
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN )
		iArray = 2;
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US || ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL )
		iArray = 3;

	for(int i =0; i < iSourceSize; i++)
	{
		if( i >= iResultSize ) break;
		szResultData[i] = szSourceData[i] ^ byKey[iArray][i%MAX_KEY];
		szResultData[i] = szResultData[i] ^ szUserKey[i%iUserKeySize];
		szResultData[i] = szResultData[i] ^ byKey[iArray][ ( (MAX_KEY-1)-(i%MAX_KEY) ) ];
		szResultData[i] = szResultData[i] ^ szUserKey[ ( (iUserKeySize-1)-(i%iUserKeySize) ) ];
	}
}

void Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey )
{
	EncryptDecryptData( szCipher, iCipherSize, szPlain, iPlainSize, szUserKey, iUserKey );

	// char를 16진수로 변경
	char szTemp[MAX_PATH*2]="";
	for(int i = 0; i < iPlainSize; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", (BYTE)szCipher[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat(szTemp, sizeof( szTemp ),  szTempHex);
	}
	StringCbCopy(szCipher, iCipherSize, szTemp);
}

void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey  )
{
	char szCharCipher[MAX_PATH*2]="";
	int  iCharChipherSize = iCipherSize/2;
	// 16진수 -> char
	int pos = 0;
	for(int i = 0; i < iCharChipherSize; i++)
	{
		char szTempOneHex[MAX_PATH]="";
		char *stopstring;
		if( pos >= iCipherSize )
			break;
		memcpy(szTempOneHex, &szCipher[pos], 2);
		pos += 2;
		if( i >= MAX_PATH*2 )
			break;
		szCharCipher[i] = (BYTE)strtol(szTempOneHex, &stopstring, 16);
	}

	EncryptDecryptData( szPlain, iPlainSize, szCharCipher, iCharChipherSize, szUserKey, iUserKey );
}

void GetMyIP( OUT char *szMyIP, IN int iSize )
{
	char szHost[MAX_PATH]="";
	gethostname( szHost, MAX_PATH );

	HOSTENT *pHost = gethostbyname( szHost );
	if( !pHost )
	{   
		StringCbPrintf( szMyIP, iSize, "%d.%d.%d.%d", 127,0,0,1 );
	}
	else
	{
		StringCbPrintf( szMyIP, iSize, "%d.%d.%d.%d", (BYTE)pHost->h_addr_list[0][0],
			(BYTE)pHost->h_addr_list[0][1],
			(BYTE)pHost->h_addr_list[0][2],
			(BYTE)pHost->h_addr_list[0][3]);
	}
}


void GetExternalIP( OUT char *szMyIP, IN int iSize )
{
	HINTERNET hInternet, hFile;
	DWORD rSize;
	char buffer[32];

	hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	hFile = InternetOpenUrl(hInternet, "http://api.ipify.org/", NULL, 0, INTERNET_FLAG_RELOAD, 0);
	InternetReadFile(hFile, &buffer, sizeof(buffer), &rSize);
	buffer[rSize] = '\0';

	StringCbPrintf(szMyIP,iSize,"%s", buffer);
	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);
}

ioClientSocket *CreateSocket( DWORD dwProcID, HWND hWnd )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsUseSocket() )
		return NULL;

	ioClientSocket *pClientSocket  = new ioClientSocket( dwProcID );

	CAutoUpgradeDlg* pDlg = NULL;
	if(AfxGetApp())
		pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	enum { MAX_LOOP = 4, MAX_CMD = 4096, TYPE_GAMESERVER_ID_LIST = 3,};
	char szCmdLine[MAX_CMD];
	ZeroMemory( szCmdLine, MAX_CMD );
	if(AfxGetApp())
		StringCbCopy( szCmdLine, MAX_CMD, AfxGetApp()->m_lpCmdLine );

	__int64 iIPAndPort = 0;

	// parsing main token
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok( szCmdLine, CMD_SECTION_TOKEN );
		else
			pPos = strtok( NULL, CMD_SECTION_TOKEN );

		if( pPos == NULL )
			break;

		if( i == TYPE_GAMESERVER_ID_LIST )
		{
			iIPAndPort = _atoi64( pPos );
			break;
		}
	}

	unsigned char szIPAndPort[MAX_PATH];
	memset( szIPAndPort, 0, MAX_PATH );
	memcpy(szIPAndPort, &iIPAndPort, 8);

	char szIP[MAX_PATH]="";
	int iPort = 0;
	ConvertIPPortIDToStr( iIPAndPort, szIP, sizeof( szIP ), iPort );

	if( !pClientSocket || !pClientSocket->InitClientTCP( hWnd, pClientSocket->GetProcID(), szIP, iPort ) )
	{
		if( pDlg && pLocal )
			pDlg->ShowErrorDlg( pLocal->GetConnectFailMent() , 227, false ); 
		LOG.PrintTimeAndLog( 0, "%s - %d TCP Socket Failed(%d)", __FUNCTION__, dwProcID, WSAGetLastError() );
	}

	return pClientSocket;
}

void ConvertIPPortIDToStr( IN __int64 iIPPortID, OUT char *szIP, IN int iIPLength, OUT int &riPort )
{
	unsigned char szIPAndPort[MAX_PATH];
	memset( szIPAndPort, 0, MAX_PATH );
	memcpy(szIPAndPort, &iIPPortID, 8);

	memset( szIP, 0, iIPLength );
	StringCbPrintf(szIP, iIPLength, "%d.%d.%d.%d", szIPAndPort[0], szIPAndPort[1], szIPAndPort[2], szIPAndPort[3]);
	riPort = 0;
	memcpy(&riPort, &szIPAndPort[4], 4);
}

// 공백 제거	JCLEE 130326
char* RTrim( char* str )
{
	char temp[1024] = {0,};
	char* end;

	strcpy( temp, str );
	end = temp + strlen( temp ) - 1;

	while( end != str && isspace(*end) )
		--end;

	*( end + 1 ) = '\0';
	str = temp;

	return str;
}

char* LTrim( char* str )
{
	char *start = str;

	while( *start != '\0' && isspace(*start) )
		++start;

	str = start;
	
	return str;
}

char* Trim( char* str )
{
	return LTrim( RTrim(str) );
}

#ifdef MAC_ADDRESS 

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void GetMacAddress( OUT char *szMacAddress, IN int iSize )
{
	PIP_ADAPTER_INFO pAdapterInfo=NULL;
	PIP_ADAPTER_INFO pAdapter = NULL;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) 
	{
		StringCbCopy( szMacAddress, iSize,  "00-00-00-00-00-00" );
		return;
	}
	
	if ( GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW )  
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) 
		{
			StringCbCopy( szMacAddress, iSize,  "00-00-00-00-00-00" );
			return;
		}
	}
	
	if( GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) != NO_ERROR )  // 첫번째 랜카드 MAC address 가져오기
	{
		FREE(pAdapterInfo);
		StringCbCopy( szMacAddress, iSize,  "00-00-00-00-00-00" );
		return;
	}

	char szPassDes[MAX_PATH]="Microsoft Virtual WiFi Miniport Adapter";
	pAdapter = pAdapterInfo;
	enum { MAX_LOOP = 100 };
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		if( !pAdapter )
		{
			StringCbCopy( szMacAddress, iSize,  "00-00-00-00-00-00" );
			break;
		}
		if( pAdapter->Type != MIB_IF_TYPE_ETHERNET )
		{
			pAdapter = pAdapter->Next;
			continue;
		}
		if( strncmp( pAdapter->Description, szPassDes, strlen( szPassDes ) )  == 0 ) // 크레온 요청 예외
		{
			pAdapter = pAdapter->Next;
			continue;
		}
		if( strcmp( pAdapter->IpAddressList.IpAddress.String, "0.0.0.0" ) == 0 )	 // 비활성 예외
		{
			pAdapter = pAdapter->Next;
			continue;
		}

		StringCbPrintf( szMacAddress, iSize,  "%0.2X-%0.2X-%0.2X-%0.2X-%0.2X-%0.2X", pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5] );
		break;
	}

	if (pAdapterInfo)
		FREE(pAdapterInfo);
}

#endif // MAC_ADDRESS

void CreateProcessWithInfo( const char* szFileName , char* szCmdLine )
{
		PROCESS_INFORMATION ProcessInfo;
		STARTUPINFO         StartupInfo;
		StartupInfo.cb               = sizeof(STARTUPINFO);
		StartupInfo.lpReserved       = NULL;
		StartupInfo.lpDesktop        = NULL;
		StartupInfo.lpTitle          = NULL;
		StartupInfo.dwFlags          = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow      = SW_SHOWNORMAL;
		StartupInfo.cbReserved2      = 0;
		StartupInfo.lpReserved2      = NULL;
#ifdef _ADMIN_
	if(strcmp(szCmdLine," ")) //앞에 공백이 없으면 명령인자가 제대로 전달되지 않습니다.
	{
		char newCmd[256] = {0,};
		sprintf( newCmd, " %s", szCmdLine );
		CreateProcess(szFileName,(LPSTR)newCmd, NULL,NULL,FALSE,0,NULL,NULL,&StartupInfo,&ProcessInfo);
	}
#else
		CreateProcess(szFileName,szCmdLine, NULL,NULL,FALSE,0,NULL,NULL,&StartupInfo,&ProcessInfo);
#endif
}
//#ifdef STEAM_ATTACH
bool CheckRunningProcess( IN CString szProcessName, IN bool bExitProcess )
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 );
	if ( (int)hSnapshot != -1 )
	{
		szProcessName.MakeUpper();
		PROCESSENTRY32 pe32 ;
		pe32.dwSize=sizeof(PROCESSENTRY32);
		CString strProcessName;
		if ( (int)hSnapshot != -1 )
		{
			PROCESSENTRY32 pe32 ;
			pe32.dwSize=sizeof(PROCESSENTRY32);
			BOOL bContinue ;
			CString strProcessName;
			if ( Process32First ( hSnapshot, &pe32 ) )
			{
				do
				{
					strProcessName = pe32.szExeFile; //strProcessName이 프로세스 이름; 
					strProcessName = strProcessName.MakeUpper();
					if( ( strProcessName.Find(szProcessName,0) != -1 ) )
					{
						if( !bExitProcess )
							return true;
/*
							Win7과 WinXP사이에서 PROCESS_ALL_ACCESS 플래그의 크기가 다름.
							그래서 7에서 빌드할 경우 XP에서 동작안함. 
							프로세스 종료 목적으로 사용할 경우 Win7과 WinXP 둘다 동작해야 한다면 PROCESS_TERMINATE로 변경
*/
						HANDLE hProcess = OpenProcess( PROCESS_TERMINATE , FALSE, pe32.th32ProcessID ); //프로세스 죽이려면
						if( hProcess )
						{
							DWORD dwExitCode;
							GetExitCodeProcess( hProcess, &dwExitCode);
							TerminateProcess( hProcess, dwExitCode);
							CloseHandle(hProcess);
							CloseHandle( hSnapshot );
							
						}
 						return true;
					}
					bContinue = Process32Next ( hSnapshot, &pe32 );
				} while ( bContinue );
			}
			CloseHandle( hSnapshot );
			return false;
		}
	}
	return false;
}
//#endif


void LoadFileInResource(int name, const char * type, DWORD& size, const char*& data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(name), type);
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

void LoadTextInResource( int name, DWORD& size, const char*& data )
{
	LoadFileInResource(name, TEXT_RESOURCE, size, data);
}

int MessageboxShow(HWND hwnd, const char * pStr, const char * pCaption, UINT uType)
{
	if(pStr && pCaption)
	{
#ifdef MULTI_BYTE_CHECK
		std::wstring strText = CA2W(pStr, COUNTRY_CODE_PAGE);
		std::wstring strCaption = CA2W(pCaption, COUNTRY_CODE_PAGE);
		return MessageBoxW( hwnd, strText.c_str(), strCaption.c_str(), uType);
#else
		return MessageBox( hwnd, pStr, pCaption, uType);
#endif
	}
	else
	{
		return MessageBox( hwnd, "Unknown Error", "Unknown Error", uType);
	}
}

void CreateResumefullzipINI()
{
	char fName[MAX_PATH]="";
	StringCbCopy( fName, sizeof(fName), "Save\\default\\resumefull.ini" );
	FILE *fp = fopen( fName, "r" );
	if( !fp )
	{
		CreateDirectory("Save",NULL);
		ZeroMemory(fName, sizeof(fName));
		StringCbCopy( fName, sizeof(fName), "Save\\default" );
		CreateDirectory(fName,NULL);
		ZeroMemory(fName, sizeof(fName));
		StringCbCopy( fName, sizeof(fName), "Save\\default\\resumefull.ini" );

		fp = fopen( fName, "w" );
		if( !fp )
			return;
	}
	fclose( fp );
}

void MsToTime(CLog& _log, DWORD dwTime)
{
	int h,m,s;
	s = dwTime / 1000;
	h = s / 3600;
	s = h > 0 ? s-(h*3600) : s;
	m = s / 60;
	s = m > 0 ? s-(m*60) : s;

	_log.PrintTimeAndLog(0,"Downloading Time : %02d:%02d:%02d ", h,m,s);
}


} //namespace Help
