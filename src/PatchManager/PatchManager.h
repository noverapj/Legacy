// PatchManager.h: interface for the CPatchManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATCHMANAGER_H__1CD08B87_B441_419A_AD10_178692F38836__INCLUDED_)
#define AFX_PATCHMANAGER_H__1CD08B87_B441_419A_AD10_178692F38836__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "ThreadModules/Thread.h"
#include "FtpApp.h"
#include "HttpApp.h"

using namespace std;

#define MAX_FILE_INFO 4
#define DELETE_FILE_LIST_NAME			"delete_file_list.cfg"
#define PATCHINFO_FILE_NAME				"server_patch.cfg"
#define VERSIONPATCH_FILE_NAME			"version_patch.cfg"
#define VERSIONPATCHINFO_FILE_NAME		"version_patch_info.txt"
#define VERSION_FIOLDER					"version"

typedef struct tag_patchinfo_file
{
	CString m_file_path;
	CString m_crc;
	CString m_file_size;
	CString m_unzip_file_size;
	bool    m_bExist;
	
	tag_patchinfo_file()
	{
		m_file_path		    = "";
		m_crc	            = "";
		m_file_size	        = "";
		m_unzip_file_size	= "";
		m_bExist            = false;
	}
}PATCHINFOFILE;

typedef struct tag_short_patchfile_info
{
	enum FILE_TYPE
	{
		FT_IOP = 0,
		FT_TXT = 1,
		FT_DDS = 2
	};
	CString m_fileName;
	CString m_filePath;
	CString m_file_patch_path;
	FILE_TYPE m_fileType;
}S_PATCHINFO;

typedef vector<S_PATCHINFO>		VSPATCHINFO;
typedef vector<PATCHINFOFILE>	VPATCHINFOFILE;
typedef vector<CString> vCString;

class CPatchManager : public Thread  
{
public:
	enum RunState
	{
		RS_UPLOAD           = 0,
		RS_CREATE_PATCHINFO = 1,
		RS_DELETELIST       = 2,
		RS_UPDATE_PATCHINFO = 3,
		RS_CHECK_FULLZIP    = 4,
		RS_NO_SERVER_FILE_DELETE = 5, 
		RS_FIX_SERVER_CFG_FILE	 = 6,
		RS_CHECK_FILE_SYNC	= 7,
		RS_SELECT_FOLDER	= 8,
		RS_SELECT_FILE		= 9,
		RS_MAKE_PATCH_PAC	= 10,
		RS_SYNC_PATCH_PAC	= 11,
		RS_MAKE_FULLZIP	= 12,
		RS_MAKE_DELETE_PATCH = 13,
	};

	enum BackUpType
	{
		BUT_NONE       = 0,
		BUT_UNCOMPRESS = 1,
		BUT_COMPRESS   = 2,
		BUT_ALL        = 3,
		BUT_UNCOMPRESS_NO_DATE      = 4,
		BUT_UNCOMPRESS_DATE_AND_ONE = 5,
	};
	//http 받을때 속성
	enum ReturnState
	{
		RETURN_NEXT,
		RETURN_ERROR,
		RETURN_STOP,
	};
	//VERSION_PATCH 파일에 대한 상태 설정(version_patch.cfg, Version_Patch_Info.ini)
	enum VERSION_FILE_INFO
	{
		VERSION_INFO_SIZE_0 = 0,
		VERSION_INFO_OK,
		VERSION_INFO_ERROR,
		VERSION_INFO_NONE,
		VERSION_INFO_CREATE,
		VERSION_PAC_SYNC
	};
	//패치 파일용
	enum PacArray
	{
		NEW_PATCH_PAC = 0,
		NEW_FULL_PAC,
	};
private:
	CFtpApp			m_FTP;
	HttpApp         m_HTTP;

	vCString        m_vDirList;
	vCString        m_vUpdateFileList;
	VPATCHINFOFILE  m_vFileList;
	VPATCHINFOFILE	m_vVersionFileList;
	vector<CString> m_vServerPatchStr;

	bool            m_bActive;
	
	int             m_GmtGap;
	
	CString         m_szBackUpRoot;
	CString         m_szUploadRoot;
	CString         m_szSyncRoot;
	
	CString         m_szIP;
	CString         m_szID;
	CString         m_szPW;
	CString         m_szPatchRoot;
	bool            m_bPassive;
	CString         m_szPatchURL;
	CString			m_szServerType;
	CString			m_szSyncLiveUrl;
	
	vCString        m_vFilePathList;
	
	int				m_iPatchDirArray;
	int				m_iFTPFileCnt;
	bool            m_bRunThread;
	ZoneType        m_eZoneType;
	RunState        m_eRunState;

	int             m_iNotSyncCnt;
	bool            m_bContinueCheckSync;

	BackUpType      m_eBackUpType;
	bool            m_bUploadLimit;
	bool            m_bAdmin;
	int				m_iPort;
public:
	bool Connect();
	bool ReConnect();
	bool IsConnect();
	bool Close();
	bool IsRunThread() const { return m_bRunThread; }

private:
	bool FTPFileList();
	bool GetFTPList(CString szCurDir);
	bool SetPatchListFromFTP(const CString &rszPath);
	void UpdatePatchListFromFTP( const CString &rszPath, const int iSize );

	void AddFileList( const char *szFileName, const PATCHINFOFILE &rkfd, bool bVersionPatchParse = false);
	void CopyPatchInfo(PATCHINFOFILE *pDst, const PATCHINFOFILE &rkSrc);	
	bool WritePatch( const CString &rszPath, bool bCheckExist );
	bool WritePatchVersionUp( const CString &rszPath, bool bCheckExist = false );
	void RunFolderFileUpdate();
	void RunUpdateSelectFile();
	bool FileCopyAndDelete(const CString &szPath, const CString& szPathDest,bool bMakeFolder);
	void CreatePatchFolder(const CString& szPathDest);
	bool CreatePatchPac( IN CString& szPacVersion, IN int iSubNum = 0 );
	void SyncPatchPac();
	void MakeFullzip();
	void MakeDeleteFileList();
	void MakeNewServerPatch(CString &szPath);
	bool FixServerCfgforDelete(CString &szDeletePath);
public:
	virtual void Run();
	void RunExit();
	
public:
	bool ConnectProcess();
	bool FileListProcess();
	void DirectoryDelete();
private:
	void RunInit();
	void RunUpload();
	void RunCreatePatchInfo();
	void RunDeleteList();
	void RunUpdatePatchInfo( bool bCheckExist );
	void FixServerCfg(bool bDelete = false);
	bool UploadProcess();
	int  DownProcess();
	bool DownProcessByUpdateList();

	bool DownloadProcessOneFile( const CString &rszFilePath );
	int GetFileSize( const CString &rszFileName );
	bool PatchInfoUploadProcess( bool bServerFile = TRUE );

	void MakeFilesList(const CString &rszRoot);
	bool DeleteWrongFilesInList();

	void SetCreateDirectory( CString szRootDir, CString szShortDir );
	void FindFileRecursive( const char *szPath);

	bool CopyFileToBackup(const CString &rszFileName, bool bDateFolder);
	bool CopyFileToBackup(const CString &rszFileName, bool bDateFolder, bool bPatchInfoFolder );
	bool CopyPatchInfoFileToBackUp(const CString &rszFileName);
	bool PacFile(const CString &rszFileName, const char *szPassword = "", const char *szComment = "");
	bool UploadFile(const CString &rszFileName, bool bPatchPac = false );
	bool BackUpFileTXTNDDS(const CString&rszFileName);
	bool DownloadFile( const CString &rszFileName, bool bVersionPatch = false );
	bool DeleteFile( const CString &rszFileName);	

	bool bDownPatchInfo( bool bVersionDown = false, bool bDelete = false );
	bool PatchParse(const char *szPath, bool bVersionPatchParse = false);
	void SetPatchListFromFile(char *pSrc,bool bVersionPatchParse = false );
	void AddFileListOncheck( const CString &rszServerPath, DWORD dwCRC, int iZipFileSize, int iUnZipFileSize);
	bool GetFTPFileInfo(IN const CString &rszFileName,OUT CString &rszServerPath, OUT tm &rTime, OUT int &iSize);
	bool UnPacPieceFile( const CString &rszDownPath);
	
	bool IsPutPacFile( CString szClientPath );
	bool IsRightVersion( const CString &rszName);

	void SetUseDisplaySec(bool bUse);
	void SetDisplayTitle( const CString &rszString);
	void GetCurrentDir( OUT CString &rString);

	bool GetFileVesrion( IN const char* szFileName, OUT char* szVersionName
		               , IN int iSize   , IN bool isProductVerion /*= false*/ );
	bool IsExistFile( const char* szFileName); // 파일 존재 확인 

	void InitFileList();
	int HTTPDownLoadProcess( const CString &szURL, const CString &rszDownloadedFullPath , const CString &rszRootDir, bool bAnnounce );
	int HttpDownLoopProcess();
	int IsSameFile(const CString &rszLocalName, const PATCHINFOFILE &rFTPPatchInfo );

	void ChangePathLikePathInfo( const CString &rszRootPath, OUT vCString &rvPathList );
	void DeleteFilesNoPathInfo( const vCString &rvLocalFileList,const VPATCHINFOFILE &rvPathInfoList, const CString &rszDeleteRoot );
	void DeleteServerFile(const CString szPath, const VPATCHINFOFILE &vFileList);
	void SetCreateDirectoryByFullPath( const char *szDir );

	bool EncryptFile( const CString &rszFileName );
	void FullZipEnd();

public:
	void SetBackUpRoot( const CString &rszBackUpRoot) { m_szBackUpRoot = rszBackUpRoot; }
	void SetUploadRoot( const CString &rszUploadRoot) { m_szUploadRoot = rszUploadRoot; }
	void SetIP( const CString &rszIP) { m_szIP = rszIP; }
	void SetID( const CString &rszID) { m_szID = rszID; }
	void SetPW( const CString &rszPW) { m_szPW = rszPW; }
	void SetPort( int iPort )		  { m_iPort = iPort;}
	void SetPatchRoot( const CString &rszPatchRoot) { m_szPatchRoot = rszPatchRoot; }
	void SetPassive(bool Passive) { m_bPassive = Passive; }
	void SetZoneType(ZoneType eZoneType) { m_eZoneType = eZoneType; }
	void SetPatchURL( const CString &rszPatchURL) { m_szPatchURL = rszPatchURL; }
	void SetVersionPatchRoot( IN const CString &rszVersionRoot ) { m_szVersionRoot = rszVersionRoot; }
	void SetBackUpType(CPatchManager::BackUpType eBackUpType) { m_eBackUpType = eBackUpType; }
	void SetUploadLimit(bool bUploadLimit) { m_bUploadLimit = bUploadLimit; }

	const CString &GetIP() const { return m_szIP; }
	const CString &GetID() const { return m_szID; }
	const CString &GetPatchRoot() const { return m_szPatchRoot; }
	const CString &GetBackUpRoot() const { return m_szBackUpRoot; }
	const CString &GetUploadRoot() const { return m_szUploadRoot; }
	const CString &GetPatchURL() const { return m_szPatchURL; }

	void SetRunState(CPatchManager::RunState RunState) { m_eRunState = RunState; }
	void SetAdmin(bool bAdmin) { m_bAdmin = bAdmin; }
public:
	CPatchManager();
	virtual ~CPatchManager();

	//ServerPatch관련
	bool RunEditPad(const CString& strPath, bool bDelete);
	void SetSyncKeynURL( CString& strKey, const CString& strPath);
private:
	ReturnState DownLoadProcess( const CString &szURL, int iRemoteFileSize, const CString &rszDownloadedFullPath );
	bool DeleteFileRepeat( const CString &strPath ) ;
	void SetCreateDirectoryByFullPath( const CString &strDirPath );
	void CheckLiveFileSync();
	bool UploadIOP(IN CString& strPath, bool bPatchPac = false );
	void RunMakePatchPac();
	bool CheckVersionPatchInfo();
	bool DownloadPatchFile( CString& LocalPath, CString& RemotePath );
	bool UploadPatchPac( IN CString& rszUploadRootFolder, IN CString& rszLocalPath );
	bool UploadPacthPactoAll();
	bool DecryptFile( const CString &rszFileName );
	bool m_bVersionUp;
	int	 m_iVersionFileState;
	int	m_iPacDivideSize;
	int m_iPacSubNum;
	CString m_szVersionRoot;
	CString m_strCurrentVersion;
	CString m_srtNewPatchFile;
	vector<CString> m_vecOldPatchInfo;
#ifdef SHIPPING
	CString GetMyIPAddr();
	bool SendSyncFile();
#endif
};

#endif // !defined(AFX_PATCHMANAGER_H__1CD08B87_B441_419A_AD10_178692F38836__INCLUDED_)
