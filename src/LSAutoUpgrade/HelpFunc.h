#pragma once

// Shipping과 Release 모두 로그를 남긴다
// 차후에 Release만 로그를 남길지 고심해 볼 것.
namespace ADMINLOG
{
	void OpenLog( int debuglv, const char *szFileName, bool is_append = false);
	void CloseAndRelease();
	void PrintTimeAndLog( int debuglv, LPSTR fmt,...);
}


#define FIREWALL_CLIENT_PATH   "%s\\lostsaga.exe"
#define FIREWALL_PROTECT_PATH  "%s\\ioprotect.exe"
#define FIREWALL_CLIENT_NAME   L"LostSaga(client)"
#define FIREWALL_PROTECT_NAME  L"LostSaga(protect)"

#define REG_SHCU HKEY_CURRENT_USER
#define REG_SHLM HKEY_LOCAL_MACHINE
#define REG_KEY_UNINSTALL "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\LostSaga_IOEntertainment"

class ioClientSocket;

namespace Help
{
	void SetUpWindowsFirewall( const char *szPath, const WCHAR  *szName );
	void RegCheckAndWrite( const char *szKey, const char *szShortValue, const char *szShortData, const char *szRoot );

	bool RegReadString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, LPTSTR lpRet, DWORD nSize);
	bool RegWriteString(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData);
	int  RegReadInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, int iDefault );
	bool RegWriteInt(HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, int iData);

	void MsToTime(CLog& _log, DWORD dwTime);

	enum 
	{
		MAX_FILE_CONTROL_LOOP = 100,
	};
	bool DeleteFileRepeat(const char *szPath);
	bool MoveFileRepeat(const char* szExistPath, const char* szNewPath);
	bool CopyFileRepeat( const char* szExistPath, const char* szNewPath );

	const char *GetAnnounceFileName(const CString &rPath);
//	bool IsRightDXVersion();

	enum ReturnValue
	{
		RV_OK       = 1,
		RV_ERROR    = 2,
		RV_ANNOUNCE = 3,
	};
	ReturnValue CheckRightHDDSpace(DWORD dwFullzipSize , OUT CString &rszErrorMsg );
	bool GetFileVesrion( IN const char* szFileName, OUT char* szVersionName, IN int iSize   , IN bool isProductVerion /*= false*/ );
	void SetCreateDirectoryByFullPath( const char *szDir );
	void SetCreateDirectory(CString szRootDir, CString szShortDir);
	bool CheckDownloadedFile( const CString &rszDownloadedFullPath , OUT CString &rszErrorMsg );
	void SetFullPath( IN const CString &szRootDir, IN const CString &szShortPath , OUT CString &szFullPath );
	void CreateShortcutOnDeskTop( const CString &szRootDir, const CString &szShortcutName, const CString &szURL );
	void CreateMenuIcon( const CString &a_szShortcutName );
	void CreateDefaultSettingINI();
	void CreateResumefullzipINI();
	DWORD GetFileSize( const char *szFileName );

	void CreateUninstallRegInfo( const CString &a_szRootDir , const char *a_szClientVersion );

	void ExcuteWeb(const char *szURL);

	void SetPieceFilePermit( bool bPermit );
	bool IsPieceFilePermit();

	void GetRandomKey( OUT char *szRandomKey, IN int iRandomKeySize );
	void EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize );
	void Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey );
	void Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey );
	void GetMyIP( OUT char *szMyIP, IN int iSize );
	void GetExternalIP( OUT char *szMyIP, IN int iSize );
	ioClientSocket *CreateSocket( DWORD dwProcID, HWND hWnd );
	void ConvertIPPortIDToStr( IN __int64 iIPPortID, OUT char *szIP, IN int iIPLength, OUT int &riPort );

	// 공백 제거	JCLEE 130326
	char* RTrim( char* str );
	char* LTrim( char* str );
	char* Trim( char* str );

//#ifdef STEAM_ATTACH
	bool CheckRunningProcess(IN const CString szProcessName,IN bool bExitProcess = false );
//#endif
#ifdef MAC_ADDRESS 
	void GetMacAddress( OUT char *szMacAddress, IN int iSize );
#endif

	void CreateProcessWithInfo( const char* szFileName , char* szCmdLine );


	void LoadFileInResource(int name, const char * type, DWORD& size, const char*& data);
	void LoadTextInResource(int name, DWORD& size, const char*& data);
	int MessageboxShow(HWND hwnd, const char * pStr, const char * pCaption, UINT uType);

}
