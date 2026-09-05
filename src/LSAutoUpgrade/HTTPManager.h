// HTTPManager.h: interface for the HTTPManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPMANAGER_H__A608E7EB_9995_48E9_B841_8FB345DF2D2A__INCLUDED_)
#define AFX_HTTPMANAGER_H__A608E7EB_9995_48E9_B841_8FB345DF2D2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;

#include "ThreadModules/Thread.h"
#include "HttpApp.h"

class CAutoUpgradeDlg;

#define MEGA							1048576
#define VERSIONPATCH_FILE_NAME			"version_patch.cfg"
#define VERSIONPATCHINFO_FILE_NAME		"version_patch_info.txt"
#define LOCAL_VERSION_INFO_FILE_NAME	"version_info.iop"

class HTTPManager : public Thread 
{
public:
	enum _RunEndType
	{
		ET_EXIT,
		ET_RUN_LAUNCHER,
		ET_WAIT_CLIENT_MSG,
		ET_ERROR_EXIT,
	};
private:
	enum FileType
	{
		FT_NONE   = -1,
		FT_PIECE  = 0,
		FT_PAC_RS = 1,
		FT_PAC_MAP= 2,
		FT_FULLZIP= 3,
		FT_AUTOUPGRADE = 4,

		FT_PAC_MAP_EN = 5,
		FT_PAC_MAP_DE = 6,
		FT_PAC_MAP_FR = 7,
		FT_PAC_MAP_IT = 8,
		FT_PAC_MAP_PL = 9,
		FT_PAC_MAP_TR = 10,
	};

	typedef struct tag_patchinfo_file
	{
		CString m_szRemoteFilePath; 
		DWORD   m_crc;
		DWORD   m_file_size;
		int     m_unzip_file_size;
		CString m_szLocalFilePath;
		FileType m_eFileType;

		tag_patchinfo_file()
		{
			m_szRemoteFilePath= "";
			m_crc	          = 0;
			m_file_size	      = 0;
			m_unzip_file_size = 0;
			m_szLocalFilePath = "";
			m_eFileType       = FT_NONE;
		}
	}PATCHINFOFILE;
	typedef struct OlldPacInfo
	{
		int iNum;
		int iSize;
	}OLD_PAC_INFO;

	typedef vector<PATCHINFOFILE> VPATCHINFOFILE;
	typedef vector<CString> vCString;
	enum ReturnState
	{
		RETURN_NEXT,
		RETURN_RETRY,
		RETURN_ERROR,
		RETURN_STOP,
	};

	enum 
	{
		MAX_FILE_INFO      = 4,
		MAX_DOWN_FILE_LOOP = 10, //갯수 수정 100 -> 10
	};

	// pac
	enum PacArray
	{
		RS_PAC    = 0, // 0~1번은 로컬에 실재로 존재하는 팩 rs.iop, map.iop
		MAP_PAC   = 1,

		MAP_EN	  = 2, // 2~7 번은 EU 을 위한 ㅠㅠ
		MAP_DE	  = 3,
		MAP_FR	  = 4,
		MAP_IT	  = 5,
		MAP_PL	  = 6,
		MAP_TR	  = 7,

		FULL_PAC  = 8, // 2~5번은 팩파일 컨트롤을 위해서 임시적으로 생성하는 팩
		NEW_PAC   = 9, 
		OLD_PAC   = 10, 
		CHECK_PAC = 11,
		PATCH_PAC = 12,
		NOT_PAC	  = 13,
		NEW_VERSION_PAC = 14,

		MAX_PAC,
		MAX_FILE_PAC = 8, // rs.iop, map.iop
	};

	HttpApp m_HTTP;
	bool    m_bActive;
	CString m_szDate;

	VPATCHINFOFILE m_vFileList;
	VPATCHINFOFILE m_vPatchPacList;
	int       m_file_count;
	char      m_szRootDir[MAX_PATH];
	bool	  m_bRunThread;
	int       m_iDisplayCnt;
	int		  m_iMaxPac;

	// ini
	CString   m_szPatchURL;
	CString   m_szLoginURL;
	CString   m_szShorcutName;
	CString   m_szAdminPatchURL;
	CString   m_szCopyVersion;
	bool      m_bAdminPatch;
	int		  m_iOptimizeBar;
	int		  m_iRetry;
	_RunEndType m_eRunEndType;

	CString   m_szErrorMsg;
	char      m_szPacFullPath[MAX_FILE_PAC][MAX_PATH];
	VPATCHINFOFILE  m_vListDeleteFile[MAX_FILE_PAC];
	vCString  m_vDeleteFileList[MAX_FILE_PAC];
	vector<int> m_vecDeleteIndex;

	DWORD     m_dwAddPacSize;
	bool      m_bWaitBeforeNewpac;
	HANDLE    m_hWaitEvent;
	CAutoUpgradeDlg *m_pAutoupDlg;
	public:
	void SetMaxGauge(const int iMax);
	void SetCurrentGauge(const int iPos);
	void AddCurrentGauge( const int iPos );
	void SetFullGauge();
	void SetOptimizeGauge(int iPersent = 0);

	HTTPManager::_RunEndType GetRunEndType() const { return m_eRunEndType; }

private:
	void SetPrintPacName( CString& szPacname, int iIndex);
	void SetText( const char *szHelp);
	void StartGame();
	void ShowErrorDlg(const char* szHelp, int iErrorNumber, bool bExitProgram = true);
	static void DisplayStateCopyFolder(LPVOID mySelf, bool isMaxCnt, int iCurrentCnt);
	void SetTotalDownloadGauge();
	bool ExtractPacFile( IN int iIndex, IN CString& rstrOutputDirName = CString("") );

	public:
	void SuspendRun( DWORD dwStopTime );
	void ResumeRun();
	void Close();
	bool IsRunThread() const { return m_bRunThread; }
	virtual void Run(); // 1

	private:
	void _RunShipping(); // 2
	void _RunAdmin();    // 2

	bool _Run();    // 3

	void _RunEnd();
	void _RunInit();
	bool _RunOptionCopyFolder(const CAutoUpgradeDlg * pDlg );
	private:
	void LoadINI();

	bool ParsePatchFile( bool bVersionInfo = false );
	void ParseOneLine(char *pSrc, bool bVersionInfo = false, bool bDelete = false);
	void AddOneFileList( const char szFile[MAX_FILE_INFO][MAX_PATH], bool bVersionInfo = false, bool bDelete = false );
	
	bool ParseDeleteFileList( const char *szPath );
	int GetDeleteFilesMegaSize();
	bool IsDeleteFile( const int iIndex , const int iPacArray );
	int  GetDeleteFileListMBWithLoad();
	int GetAllGarbageMega();
	
	ReturnState UpdateFileLoop();
	bool ResponseServerCheck();
	const PATCHINFOFILE *GetUpdateFile( bool bShowPercent = false, bool bCheckOverlapFile = false);
	bool PieceFileCheck( IN const CString &rszDownloadedFullPath , IN DWORD dwRemoteFileSize, OUT DWORD &rdwDownloadedSize );
	ReturnState UpdateOneFile(const PATCHINFOFILE *pNext );
	ReturnState DownLoadProcess( const CString &szURL ,int iRemoteFileSize, const CString &rszDownloadedFullPath , OUT DWORD &rdwDownloadedSize);

	ReturnState FileCopy(const PATCHINFOFILE *pNext, const CString &rszClientFullPath, const CString &rszDownloadedFullPath);
	ReturnState SetupFull(const CString &rszDownlaodFullPath,const CString &rszClientFullPath );

	void SendLOG( const char *szLog, int iErrorNumber );

	// pac
	private:
	bool IsPutPacFileByPath( CString szRemoteFilePath );
	int	 IsMapPacFileByPath( CString szRemoteFilePath );
	bool IsPutPacFile( const FileType eFileType );
	int IsMapPacFile( const FileType eFileType );

	bool InitPac();
	ReturnState UnFull(const CString &rszDownloadedFullPath);
	bool ProcessCheckPac();
	bool CheckPac( const char* szFullPath, const int iDisplayCnt , const int iMaxDisplayCnt);
	bool UnPac(const char* szPacName, const int iDisplayCnt , const int iMaxDisplayCnt);
	bool AddPacFile(const PATCHINFOFILE *pFile,const CString &rszDownloadedFullPath);
	bool UnPacPieceFile(const CString &rszDownloadedFullPath);

	bool ProcessNewPacFile();
	bool IsCreateNewPacFile(IN const int iCurGarbageMega );
	bool CreateAllNewPacFile();
	bool CreateNewPacFile( IN const int iPacArray, IN const char * szOldPacPath, IN const char *szNewPacPath );
	bool OpenAllBigPacForGarbageSize();
	void SetTextWaitNewPac( CAutoUpgradeDlg * pDlg );

	void SavePacSizeAfterOptimize();
	bool IsModifiedPacSizeAfterOptimize();
	bool IsManualMaxOptimizeMega( int iCurGarbageMega );

	bool DecryptFile( const CString &rszFileName );
	void CopyVersionupFiles();

	protected:
	// FilePathList
	typedef std::vector<CString> vCString;
	vCString m_vFilePathList;
	void MakeFilesList(const CString &rszRoot);
	void FindFileRecursive( const char *szPath);

	public:
	bool DownLoadOneFile( const CString &szURL ,int iRemoteFileSize, const CString &rszDownloadedFullPath , OUT DWORD &rdwDownloadedSize);


	HTTPManager();
	virtual ~HTTPManager();

#ifdef SHIPPING
private:
	bool	CheckPatchPac();
	typedef pair<string, DWORD> Overlapdata;
	vector<Overlapdata> m_vecOverlap[MAX_PAC];
	vector<CString>		m_vecSkipFileList;
	void DeleteGarbage();
	bool m_bOverlapCheck;
	bool CheckOverlapFile(const PATCHINFOFILE * pNext);
	bool DownloadFile( IN const CString& rszURL, IN const CString& rszFullPath, IN int iErrorCode, OUT DWORD& rdwDownloadSize );
	bool UpdatePatchFile();
	bool PerDownload( IN const CString& rszURL, IN const CString& rszFullPath, IN int iErrorCode, OUT DWORD& rszDownloadSize, IN CString& strPrePacSize );
	int  GetPacType( IN CString& rszFilePath );
	void GetLocalVersion( OUT CString& strVersion );
	void SetLocalVersion( IN CString strVersion );
	bool EncryptFile( const CString &rszFileName );
	CString m_strPrePacSize;
	CString m_strPatchURL;
	CString m_strLocalVersion;
	CString m_strCurrentPatchVersion;
	CString m_strCurrentPatchSub;
	CString m_strDownloadPossibleVersion;
	bool	m_bShowGauge;
	vector<OLD_PAC_INFO> m_vecOldPacInfo;
	vector<CString> m_vecSkipPac;
#endif
};

#endif // !defined(AFX_HTTPMANAGER_H__A608E7EB_9995_48E9_B841_8FB345DF2D2A__INCLUDED_)
