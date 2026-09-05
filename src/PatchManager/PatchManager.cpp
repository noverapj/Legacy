// PatchManager.cpp: implementation of the CPatchManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "PatchManager.h"
#include "InterfaceManager.h"
#include "AutoUpgradeDlg.h"
#include "EditDlg.h"
#include "Crc32Static.h"
#include <strsafe.h>
#include "local\iolocalparent.h"
#include "AutoUpgradeDlg.h"

#include <fstream>
#ifdef SHIPPING
#include <Wininet.h>
#include <atlcomtime.h>
#include <winsock2.h>
#include <Rpc.h>
#pragma comment (lib, "wininet.lib")
#endif
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPatchManager::CPatchManager( )
{
	m_vDirList.reserve(1000);
	m_vFileList.reserve(1000);
	m_vFilePathList.reserve(1000);

	m_bActive        = false;
	m_bPassive       = false;
	m_iPatchDirArray = -1;
	m_iFTPFileCnt    = 0;
	m_bRunThread     = false;
	m_eZoneType      = ZONE_TYPE_OFFICE;
	m_eRunState      = RS_UPLOAD;
	m_iNotSyncCnt    = 0;
	m_bContinueCheckSync = false;
	m_eBackUpType    = BUT_NONE;
	m_bUploadLimit    = false;
	m_bAdmin         = false;
	m_iPort			 = NULL;
	m_bVersionUp	 = false;
	m_iVersionFileState = VERSION_INFO_NONE;
	m_iPacDivideSize = 0;
	m_iPacSubNum	 = 0;
}

CPatchManager::~CPatchManager()
{
	m_FTP.close();
	m_vDirList.clear();
	m_vFileList.clear();
	m_vFilePathList.clear();
	m_vUpdateFileList.clear();
}

bool CPatchManager::Connect()
{
	return  m_FTP.connect((LPCTSTR)m_szIP,(LPCTSTR)m_szID,(LPCTSTR)m_szPW, m_iPort);
}

bool CPatchManager::Close()
{
	m_bActive = false;
	return true;
}

bool CPatchManager::IsConnect()
{
	return m_FTP.isconnect();
}

bool CPatchManager::ReConnect()
{
	m_FTP.close();
	return Connect();
}

bool CPatchManager::ConnectProcess()
{
	//연결.
	if(!Connect()) 
	{
		int count = 1;
		while(!ReConnect())
		{
			if(!m_bActive)
				return false;
			count++;
			CString szHelp;
			szHelp.Format("FTP ReConnecting %d....",count);
			InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
			Sleep(100);
		}
	}
	if(!IsConnect())
		return false;	

	return true;
}

bool CPatchManager::FileListProcess()
{
	m_iFTPFileCnt = 0;
	m_iPatchDirArray=-1;
	m_vDirList.clear();
	if(!GetFTPList(m_szPatchRoot))
		return false;
	m_vDirList.clear();

	return true;
}

void CPatchManager::Run()
{
	RunInit();

	if( m_eRunState == RS_UPLOAD )
		RunUpload();
	else if ( m_eRunState == RS_CREATE_PATCHINFO )
		RunCreatePatchInfo();
	else if( m_eRunState == RS_DELETELIST )
		RunDeleteList();
	else if( m_eRunState == RS_UPDATE_PATCHINFO ) 
		RunUpdatePatchInfo( false );
	else if( m_eRunState == RS_CHECK_FULLZIP )
		RunUpdatePatchInfo( false );
	else if( m_eRunState == RS_NO_SERVER_FILE_DELETE )
		RunUpdatePatchInfo( true );
	else if( m_eRunState == RS_FIX_SERVER_CFG_FILE )
		FixServerCfg();
	else if( m_eRunState == RS_CHECK_FILE_SYNC )
		CheckLiveFileSync();
	else if( m_eRunState == RS_SELECT_FILE )
		RunUpdateSelectFile();
	else if( m_eRunState == RS_SELECT_FOLDER )
		RunFolderFileUpdate();
	else if( m_eRunState == RS_MAKE_PATCH_PAC )
		RunMakePatchPac();
	else if( m_eRunState == RS_SYNC_PATCH_PAC )
		SyncPatchPac();
	else if( m_eRunState == RS_MAKE_DELETE_PATCH )
		MakeDeleteFileList();
	else if( m_eRunState == RS_MAKE_FULLZIP )
		MakeFullzip();
	
	RunExit();
	if(m_eRunState == RS_MAKE_FULLZIP)
		FullZipEnd();
		
	
}

bool CPatchManager::GetFTPList( CString szCurDir )
{
	nsFTP::TSpFTPFileStatusVector vList;
	vList.reserve(100);

	if(!m_bActive)
		return false;
	if(!m_FTP.GetFileList(szCurDir, vList, m_bPassive))
		return false;

	CString szHelp;
	for( nsFTP::TSpFTPFileStatusVector::iterator it=vList.begin(); it!=vList.end(); ++it )
	{
		if((*it)->IsDot()) continue;
		if((*it)->IsCwdPossible()) // dir
		{
			CString FullDir = szCurDir;
			FullDir += (*it)->Name().c_str();
			FullDir += "/";
			m_vDirList.push_back(FullDir);
			szHelp.Format("Read FTP Dir: %s",(LPCTSTR)FullDir);
		}
		else // file
		{
			CString FullFilePath = szCurDir + (*it)->Name().c_str();   
			FullFilePath = FullFilePath.Right( FullFilePath.GetLength() - strlen((LPCTSTR)m_szPatchRoot));
			int iSize    = (*it)->Size();

			if( m_eRunState == RS_CREATE_PATCHINFO )
				SetPatchListFromFTP(FullFilePath);
			else
				UpdatePatchListFromFTP( FullFilePath, iSize );

			m_iFTPFileCnt++;
			szHelp.Format("Read FTP File: %d/%s",m_iFTPFileCnt,FullFilePath);
		}
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
	}
	vList.clear();

	int iSize = m_vDirList.size();
	m_iPatchDirArray++;
	if(m_iPatchDirArray < iSize && m_iPatchDirArray >= 0)
		GetFTPList(m_vDirList[m_iPatchDirArray]);
	return true;
}

void CPatchManager::CopyPatchInfo( PATCHINFOFILE *pDst, const PATCHINFOFILE &rkSrc )
{
	if(pDst == NULL) return;
	pDst->m_file_path	      = rkSrc.m_file_path;
	pDst->m_crc	              = rkSrc.m_crc;
	pDst->m_file_size         = rkSrc.m_file_size;
	pDst->m_unzip_file_size   = rkSrc.m_unzip_file_size;
}

bool CPatchManager::SetPatchListFromFTP( const CString &rszPath )
{
	PATCHINFOFILE fd;
	fd.m_file_path	= rszPath;
	AddFileList((LPCTSTR)rszPath, fd);
	return true;
}

void CPatchManager::UpdatePatchListFromFTP( const CString &rszPath, const int iSize )
{
	CString szPatchInfo = PATCHINFO_FILE_NAME;
	szPatchInfo += ".iop";

	if( rszPath == szPatchInfo ) 
		return;

	int iMax = m_vFileList.size();
	for(int i = 0; i < iMax; i++)
	{
		if(m_vFileList[i].m_file_path.CompareNoCase( rszPath ) == 0)
		{
			if( m_eRunState == RS_NO_SERVER_FILE_DELETE || m_eRunState == RS_DELETELIST )
				m_vFileList[i].m_bExist = true;

			int iCurSize = atoi( (LPCTSTR) m_vFileList[i].m_file_size );
			if( iCurSize != iSize )
			{
				if( m_eRunState == RS_UPDATE_PATCHINFO )
				{
					// 풀집은 사이즈 갱신
					if( m_vFileList[i].m_file_path.Find( "full.zip") != -1 )
					{
						m_vFileList[i].m_file_size.Format( "%u",iSize );
						LOG.PrintTimeAndLog(0, "%s size  %d->%d" , ( LPCTSTR ) rszPath, iCurSize, iSize );
					}
					else
					{
						m_vUpdateFileList.push_back( rszPath );
						LOG.PrintTimeAndLog(0, "%s size  %d->%d download" , ( LPCTSTR ) rszPath, iCurSize, iSize );
					}
				}
				else if( m_eRunState == RS_CHECK_FULLZIP )
				{
					// 풀집은 사이즈 갱신
					if( m_vFileList[i].m_file_path.Find( "full.zip") != -1 )
					{
						m_vFileList[i].m_file_size.Format( "%u",iSize );
						LOG.PrintTimeAndLog(0, "%s size  %d->%d" , ( LPCTSTR ) rszPath, iCurSize, iSize );
					}
				}
			}
			return;
		}
	}

	if( m_eRunState == RS_UPDATE_PATCHINFO )
	{
		m_vUpdateFileList.push_back( rszPath );
		LOG.PrintTimeAndLog(0, "Server Only File %s" , ( LPCTSTR ) rszPath );
	}
}

bool CPatchManager::WritePatch( const CString &rszPath, bool bCheckExist )
{
	FILE    *file = NULL;
	file = ::fopen((LPCTSTR)rszPath,"wt");
	if(file == NULL) return false;

	bool bDeleteFileList = false;
	if( rszPath.Find( DELETE_FILE_LIST_NAME ) != -1 )
		bDeleteFileList = true;

	VPATCHINFOFILE *vecPatchInfo = NULL;
	if( -1 != rszPath.Find( VERSIONPATCH_FILE_NAME ) )
	{
		vecPatchInfo = &m_vVersionFileList;
	}
	else if( -1 != rszPath.Find(PATCHINFO_FILE_NAME) )
	{
		vecPatchInfo = &m_vFileList;
	}

	int iSize = vecPatchInfo->size();
	for(int i = 0;i < iSize;i++)
	{
		if( !bDeleteFileList )
		{
			if( bCheckExist )
			{
				if( !(*vecPatchInfo)[i].m_bExist )
				{
					CString szHelp;
					szHelp.Format("No server file: %s", (LPCTSTR)(*vecPatchInfo)[i].m_file_path );
					InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
					LOG.PrintTimeAndLog(0, "No server file: %s", (LPCTSTR)(*vecPatchInfo)[i].m_file_path );
					continue;
				}
			}
		}

		if( (*vecPatchInfo)[i].m_file_path.IsEmpty() )
			continue;

		(*vecPatchInfo)[i].m_file_path.MakeLower();

		CString szList;
		szList = "[";
// 		if( bDeleteFileList )
// 		{
// 			szList += (*vecPatchInfo)[i].m_file_path;
// 		}
// 		else
		{
			szList += (*vecPatchInfo)[i].m_file_path + ":";
			szList += (*vecPatchInfo)[i].m_crc + ":";
			szList += (*vecPatchInfo)[i].m_file_size + ":";
			szList += (*vecPatchInfo)[i].m_unzip_file_size;
		}
		szList += "]";

		::fprintf(file, "%s\n",(LPCTSTR)szList);
	}
	::fclose( file );	
	return true;
}

void CPatchManager::FindFileRecursive( const char *szPath )
{
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;
	char			fname[MAX_PATH];
	BOOL			bResult = TRUE;
	char			drive[_MAX_DRIVE];
	char			dir[MAX_PATH];
	char			newpath[MAX_PATH];	

	hSrch = FindFirstFile(szPath, &wfd);

	if(hSrch == INVALID_HANDLE_VALUE)
		return;

	while(bResult)
	{
		_splitpath(szPath, drive, dir, NULL, NULL);
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(wfd.cFileName[0] != '.')
			{
				wsprintf(newpath, "%s%s%s\\*.*",drive, dir, wfd.cFileName);
				FindFileRecursive(newpath);
			}
		}
		else
		{			
			wsprintf(fname, "%s%s%s", drive, dir, wfd.cFileName);
			strlwr(fname);
			m_vFilePathList.push_back(fname);	
		}
		bResult = FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);
}

void CPatchManager::MakeFilesList( const CString &rszRoot )
{
	m_vFilePathList.clear();
	if(!rszRoot.IsEmpty())
	{
		CString szFindPath = rszRoot.Left(rszRoot.GetLength() - 1); 
		CreateDirectory((LPCTSTR)szFindPath,NULL);
		FindFileRecursive((LPCTSTR)szFindPath);
	}
}

bool CPatchManager::CopyFileToBackup( const CString &rszFileName, bool bDateFolder, bool bPatchInfoFolder )
{
	if(m_szBackUpRoot.IsEmpty()) 
	{
		InterfaceManager::GetInstance()->SetAnnounceFirstLine("the Backup Folder is empty.");
		return false;	
	}

	// 날짜
	char szDate[MAX_PATH]="";
	if( bDateFolder )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		int iDate = (st.wYear * 10000) + (st.wMonth * 100) + st.wDay;
		StringCbPrintf( szDate, sizeof( szDate ), "%d\\", iDate );
	}

	if( bPatchInfoFolder )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		int iDate = (st.wYear * 10000) + (st.wMonth * 100) + st.wDay;
		int iTime = (st.wHour * 10000) + (st.wMinute * 100) + st.wSecond;
		StringCbPrintf( szDate, sizeof( szDate ), "ServerPatchInfo\\%d%d\\", iDate, iTime );
	}

	CString szShortDir = rszFileName;
	szShortDir = szShortDir.Right(szShortDir.GetLength() - (m_szUploadRoot.GetLength()));
	szShortDir = szDate + szShortDir;

	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)szShortDir,NULL,dir,fname,ext);


	SetCreateDirectory(m_szBackUpRoot, dir);
	CString szBackUpPath = m_szBackUpRoot;
	szBackUpPath += szShortDir;
	SetFileAttributes((LPCTSTR)szBackUpPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szBackUpPath);
	SetFileAttributes((LPCTSTR)rszFileName,FILE_ATTRIBUTE_NORMAL);
	CString sz = rszFileName;
	if(!::CopyFile((LPCTSTR)rszFileName, (LPCTSTR)szBackUpPath, false)) return false;

	return true;
}

bool CPatchManager::CopyFileToBackup( const CString &rszFileName, bool bDateFolder )
{
	if(m_szBackUpRoot.IsEmpty()) 
	{
		InterfaceManager::GetInstance()->SetAnnounceFirstLine("the Backup Folder is empty.");
		return false;	
	}

	// 날짜
	char szDate[MAX_PATH]="";
	if( bDateFolder )
	{
		SYSTEMTIME st;
		GetLocalTime( &st );
		int iDate = (st.wYear * 10000) + (st.wMonth * 100) + st.wDay;
		StringCbPrintf( szDate, sizeof( szDate ), "%d\\", iDate );
	}

	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)rszFileName,NULL,dir,fname,ext);
	SetCreateDirectory(m_szBackUpRoot, szDate);
	CString szBackUpPath = m_szBackUpRoot + szDate + fname + ext;
	SetFileAttributes((LPCTSTR)szBackUpPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szBackUpPath);
	SetFileAttributes((LPCTSTR)rszFileName,FILE_ATTRIBUTE_NORMAL);

	if(!::CopyFile((LPCTSTR)rszFileName, (LPCTSTR)szBackUpPath, false)) 
		return false;
	char szMsg[MAX_PATH] ="";
	StringCbPrintf(szMsg, sizeof(szMsg), "File BackUp complete! - %s", szBackUpPath);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szMsg);
	LOG.PrintTimeAndLog(0,"File BackUp complete! - %s", szBackUpPath);
	return true;
}

bool CPatchManager::CopyPatchInfoFileToBackUp( const CString &rszFileName )
{
	if( -1 == rszFileName.Find(PATCHINFO_FILE_NAME) )
		return false;

	CString	strPatchInfoSavePath;
	char szRootDir[MAX_PATH] = "";
	::GetCurrentDirectory(MAX_PATH,szRootDir);
	char szPath[MAX_PATH]="";
	StringCbPrintf(szPath, sizeof(szPath), "%s\\pm.ini", szRootDir);
	ioINILoaderAU   INILoader;
	INILoader.SetFileName(szPath);
	char szPatchInfoBackUpDir[MAX_PATH] = "";
	INILoader.LoadString("Zone2","BackFolder","",szPatchInfoBackUpDir,MAX_PATH);
	strPatchInfoSavePath = szPatchInfoBackUpDir;
	strPatchInfoSavePath += PATCHINFO_FILE_NAME;
	strPatchInfoSavePath += ".iop";
//	if( 0 < strPatchInfoSavePath.Find("Client\\") - 6 )
//	{
		if(!::CopyFile((LPCTSTR)rszFileName, (LPCTSTR)strPatchInfoSavePath, false)) return false;
//	}
	else
		return false;

	return true;
}

void CPatchManager::SetCreateDirectory( CString szRootDir, CString szShortDir )
{
	const TCHAR *pShortDir = (LPCTSTR)szShortDir;

	TCHAR createDir[MAX_PATH*2] = "";
	_tcscpy(createDir,szRootDir);
	int len = _tcslen(createDir);

	while (*pShortDir != _T('\0')) 
	{
		if( *pShortDir ==_T('/') || 
			*pShortDir ==_T('\\') )
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

bool CPatchManager::PacFile( const CString &rszFileName, const char *szPassword /*= ""*/, const char *szComment /*= ""*/ )
{
	CString szServerPath = rszFileName;
	szServerPath = szServerPath.Right(szServerPath.GetLength() - (m_szUploadRoot.GetLength()));

	CString szHelp;
	szHelp.Format("Pack : %s", (LPCTSTR)szServerPath);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);

	CString szTemp = rszFileName;
	szTemp.MakeLower();
	szTemp.MakeReverse();

	if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
	{
		if( !EncryptFile( rszFileName ) )
			return false;
	}

	if(!g_Pac.PacPieceFile((LPCTSTR)rszFileName, szPassword, szComment ))
	{
		CString szHelp;
		szHelp.Format("Fail Pack Upload file : %s", (LPCTSTR)szServerPath);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}

	if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
	{
		if( !EncryptFile( rszFileName ) )
			return false;
	}

	SetUseDisplaySec(false);

	return true;	
}

bool CPatchManager::UploadFile( const CString &rszFileName, bool bPatchPac )
{
	CString szServerPath = rszFileName;
	CString strOldPatchRoot;
//TEST	szServerPath.Replace('\\','/');
	if( bPatchPac )
	{
		int iPoint = szServerPath.ReverseFind('/');
		szServerPath = szServerPath.Right( szServerPath.GetLength() - (iPoint + 1 ));

		if( szServerPath.Find(".dds") != -1 )
		{
			strOldPatchRoot = m_szPatchRoot;
			m_szPatchRoot = "/Client/_map/resource/texture/";
		}
		if(szServerPath.Find(".txt") != -1)
		{
			strOldPatchRoot = m_szPatchRoot;
			m_szPatchRoot = "/Client/_map/resource/text/";
		}
	}
	else
	{
		szServerPath = szServerPath.Right(szServerPath.GetLength() - (m_szUploadRoot.GetLength()));
	}


	CString szHelp;
	szHelp.Format("Upload : %s", (LPCTSTR)szServerPath);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);

	int iCnt = 10;
	while( iCnt > 0)
	{
		if(!m_FTP.UploadFile(rszFileName, m_szPatchRoot+szServerPath, m_bPassive))
		{
			m_FTP.Abort();
			Sleep(1000);
			ReConnect();
			LOG.PrintTimeAndLog(0, "업로드 RECONNECT : %s", (LPCTSTR)rszFileName );
		}
		else
		{
			break;
		}

		iCnt--;
		Sleep(1000);
	}
	if( !strOldPatchRoot.IsEmpty() )
	{
		m_szPatchRoot = strOldPatchRoot;
	}

	if(iCnt == 0)
	{
		CString szHelp;
		szHelp.Format("Fail Uploading files : %s", (LPCTSTR)szServerPath);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}

	SetUseDisplaySec(false);

	return true;		
}


bool CPatchManager::BackUpFileTXTNDDS( const CString&rszFileName )
{
	CString szPathFile = rszFileName;

	int iPoint = szPathFile.ReverseFind('\\');
	szPathFile = szPathFile.Right( szPathFile.GetLength() - (iPoint + 1 ));

	CString	strPatchInfoSavePath;
	char szRootDir[MAX_PATH] = "";
	::GetCurrentDirectory(MAX_PATH,szRootDir);
	char szPath[MAX_PATH]="";
	StringCbPrintf(szPath, sizeof(szPath), "%s\\pm.ini", szRootDir);
	ioINILoaderAU   INILoader;
	INILoader.SetFileName(szPath);
	char szPatchInfoBackUpDir[MAX_PATH] = "";
	INILoader.LoadString("PatchInfo","CurrentPatchPath","",szPatchInfoBackUpDir,MAX_PATH);
	strPatchInfoSavePath = szPatchInfoBackUpDir;
	if( szPathFile.Find(".dds") != -1 )
	{
		strPatchInfoSavePath += "_map\\resource\\texture\\";
	}
	else if(szPathFile.Find(".txt") != -1)
	{
		strPatchInfoSavePath += "_map\\resource\\text\\";
	}
	else
		return false;

	strPatchInfoSavePath += szPathFile;
	CreatePatchFolder(strPatchInfoSavePath);
	if(!::CopyFile((LPCTSTR)rszFileName, (LPCTSTR)strPatchInfoSavePath, false))
	{
		LOG.PrintTimeAndLog(0, "TXT 및 DDS 파일 백업 실패 : %s", (LPCTSTR)rszFileName);
		return false;		
	}

	return true;
}

bool CPatchManager::DownloadFile( const CString &rszFileName, bool bVersionPatch )
{
	CString szHelp;
	szHelp.Format("Download : %s", (LPCTSTR)rszFileName);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	CString szServerPath = rszFileName;
	szServerPath = szServerPath.Right(szServerPath.GetLength() - 7); // config/ 만큼 삭제함.

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);

	int iCnt = 10;
	int iRemoteSize = 0;
	while(iCnt > 0)
	{
		if(!m_FTP.DownloadFile( rszFileName, m_szPatchRoot+ szServerPath, m_bPassive, &iRemoteSize ))
		{
			if( bVersionPatch && iRemoteSize == 0 && m_iVersionFileState != VERSION_PAC_SYNC )
			{
				CString szText;
				szText.Format("Can't not found File : %s\nRemote FileSize is 0", (LPCTSTR)rszFileName);
				m_iVersionFileState = VERSION_INFO_SIZE_0;
				return false;
			}
			m_FTP.Abort();
			Sleep(1000);
			ReConnect();
			LOG.PrintTimeAndLog(0, "다운로드 RECONNECT : %s", (LPCTSTR)rszFileName);
		}
		else
		{
			break;
		}

		iCnt--;
		Sleep(1000);
	}

	if( iCnt == 0)
	{
		CString szHelp;
		szHelp.Format("Fail Downloading files : %s", (LPCTSTR)rszFileName);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		if( bVersionPatch ) m_iVersionFileState = VERSION_INFO_ERROR;
		return false;
	}

	SetUseDisplaySec(false);
	if( bVersionPatch ) 
		m_iVersionFileState = VERSION_INFO_OK;
	return true;		
}


bool CPatchManager::DeleteFile( const CString &rszFileName )
{
	int iCnt = 100;
	while(iCnt > 0)
	{
		SetFileAttributes((LPCTSTR)rszFileName,FILE_ATTRIBUTE_NORMAL);
		if(::DeleteFile((LPCTSTR)rszFileName))
			break;
		Sleep(1);
		iCnt--;
	}
	if(!iCnt)
		return false;

	return true;		
}

bool CPatchManager::DeleteWrongFilesInList()
{
	if(m_vFilePathList.empty()) return true;

	for( vCString::iterator itor = m_vFilePathList.begin(); itor != m_vFilePathList.end(); )
	{
		CString temp = *itor;
		temp.MakeLower();

		bool isWrongFile = false;

		if(temp.Find("full.zip") != -1)
			isWrongFile = false;
		else if(temp.Find("autoupgrade.exe") != -1 )
		{
			isWrongFile = true;
		}
		else
		{
			temp.MakeReverse();
			if(m_eZoneType != ZONE_TYPE_SHIPPING)
			{
				if( ( !temp.Find("poi.") &&  temp.Find("poi.gfc.tsil_elif_eteled") == -1 ) || // delete file list는 iop라도 잘못된 파일 아니다.
					!temp.Find("piz.") || 
					!temp.Find("bil.") ||
					( !temp.Find("gfc.") && temp.Find( "gfc.efi" ) == -1 ) ) // ife.cfg 파일은 cfg라도 잘못된 파일이 아니다.
					isWrongFile = true;
			}
			else
			{
				if(( !temp.Find("poi.") &&  temp.Find("poi.gfc.tsil_elif_eteled") == -1 ) ||  // delete file list는 iop라도 잘못된 파일 아니다.
					!temp.Find("piz.") || 
					!temp.Find("bil.") || 
					!temp.Find("bdp.") ||
					( !temp.Find("gfc.") && temp.Find( "gfc.efi" ) == -1 ) ) // ife.cfg 파일은 cfg라도 잘못된 파일이 아니다.
					isWrongFile = true;
			}

			temp.MakeReverse();
			if( !isWrongFile && m_bUploadLimit )
			{	
				if( temp.Find( "_map\\resource\\text\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("txt.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\texture\\" ) != -1 || temp.Find( "_map\\resource\\texture\\" ) != -1 
					|| temp.Find( "_map_en\\resource\\texture\\" ) != -1 || temp.Find( "_map_de\\resource\\texture\\" ) != -1
					|| temp.Find( "_map_fr\\resource\\texture\\" ) != -1 || temp.Find( "_map_it\\resource\\texture\\" ) != -1
					|| temp.Find( "_map_pl\\resource\\texture\\" ) != -1 || temp.Find( "_map_tr\\resource\\texture\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("sdd.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\effect\\" ) != -1       || temp.Find( "resource\\material\\" ) != -1       || temp.Find( "resource\\model\\" ) != -1 ||
					temp.Find( "_map\\resource\\effect\\" ) != -1 || temp.Find( "_map\\resource\\material\\" ) != -1 || temp.Find( "_map\\resource\\model\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("txt.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\mesh\\" ) != -1 || temp.Find( "_map\\resource\\mesh\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("smc.") != 0 && temp.Find("hsm.") != 0 && temp.Find("gde.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\skeleton\\" ) != -1 || temp.Find( "_map\\resource\\skeleton\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("lks.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\font\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("ftt.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\animation\\" ) != -1 ||  temp.Find( "_map\\resource\\animation\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("ina.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "resource\\swf\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("fws.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "_map\\resource\\wave\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("ggo.") != 0 && temp.Find("vaw.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "config\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("ini.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "xml\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("lmx.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "scripts\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("aul.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "shader\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("xf.") != 0 && temp.Find("oxf.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "info\\" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("poi.gfc.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else if( temp.Find( "" ) != -1 )
				{
					temp.MakeReverse();
					if( temp.Find("exe.") != 0 && temp.Find("lld.") != 0 )
					{
						isWrongFile = true;
					}
				}
				else
				{
					isWrongFile = true;
				}
			}
		}

		if(isWrongFile)
		{
			CString szHelp;
			szHelp.Format("Delete Wrong File : %s", (LPCTSTR)*itor);
			LOG.PrintTimeAndLog( 0, "%s", (LPCTSTR)szHelp );
			InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

			// delete 
			int iCnt = 100;
			while(iCnt > 0)
			{
				SetFileAttributes((LPCTSTR)*itor,FILE_ATTRIBUTE_NORMAL);
				if(::DeleteFile((LPCTSTR)*itor))
					break;
				Sleep(1);
				iCnt--;
			}
			if(!iCnt)
			{
				CString szHelp;
				szHelp.Format("Fail Delete Error file : %s", (LPCTSTR)*itor);
				InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
				return false;
			}

			m_vFilePathList.erase(itor);
		}
		else
			++itor;
	}

	return true;
}
bool CPatchManager::UploadProcess()
{
	MakeFilesList(m_szUploadRoot);

	if(m_vFilePathList.empty())
		return false;

	// 	if(!DeleteWrongFilesInList())		
	// 		return false;

	int iMax = m_vFilePathList.size();
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);

	CreateDirectory((LPCTSTR)m_szBackUpRoot,NULL);

	for(int i =0; i < iMax; i++)
	{
		if(!m_bActive)
			return false;

		if( IsRightVersion(m_vFilePathList[i]) == false )
		{
			AfxMessageBox("파일버전이 잘못되었습니다.\n\n관리자나 유저 모드 파일이 맞는지\n\n확인해 주세요.");
			LOG.PrintTimeAndLog(0, "FAIL - IsRightVersion:%s", (LPCTSTR)m_vFilePathList[i]);
			return false;
		}

		bool isFullZip = false;
		if(m_vFilePathList[i].Find("full.zip") != -1)
			isFullZip = true;

		if(isFullZip == false)
		{
			CString szShortPath = m_vFilePathList[i];
//TEST			szShortPath.Replace("\\", "/");
			szShortPath = szShortPath.Right(szShortPath.GetLength() - (m_szUploadRoot.GetLength()));

			char szPacPassword[MAX_PASSWORD+1]="";
			char szPacComment[MAX_PATH]="";
			if( IsPutPacFile( szShortPath ))
			{
				enum { PASSWORD_TYPE = 1, };
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal )
					pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );

				StringCbPrintf( szPacComment, sizeof( szPacComment ), "%d", PASSWORD_TYPE );
			}

			if(!PacFile(m_vFilePathList[i], szPacPassword, szPacComment ) )
			{
				LOG.PrintTimeAndLog(0, "팩 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
				return false;
			}
		}

		CString szPacName = m_vFilePathList[i];

		if(isFullZip == false)
			szPacName += ".iop";

		if(!UploadFile(szPacName))
		{
			LOG.PrintTimeAndLog(0, "업로드 실패 : %s", (LPCTSTR)szPacName);
			return false;
		}

		CString szServerPath = szPacName;
		szServerPath.Replace("\\", "/");
		szServerPath = szServerPath.Right(szServerPath.GetLength() - (m_szUploadRoot.GetLength()));

		// get crc / filesize
		DWORD dwCRC     = 0;
		int   iUnZipFileSize = 0;
		int   iZipFileSize = GetFileSize(szPacName);

		if(isFullZip == false)
		{
			iUnZipFileSize = GetFileSize(m_vFilePathList[i]);

			if(IsPutPacFile(szServerPath))
			{
				enum { PASSWORD_TYPE = 1, };
				char szPacPassword[MAX_PASSWORD+1]="";
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal )
					pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );
				dwCRC = g_Pac.GetPieceFileCRC( (LPCTSTR)szPacName, szPacPassword );
			}
			else
			{
				if(CCrc32Static::FileCrc32Streams((LPCTSTR)m_vFilePathList[i], dwCRC) != 0 )
				{
					LOG.PrintTimeAndLog(0, "CRC 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
					return false;
				}
			}
		}
		//

		// 관리자 모드이면 이전 파일과 현재 파일이 같으면 안내, 해외 패치 전달시 에러가 발생하므로 파일을 수정하여 파일 사이즈 변경 필요.
		if( m_bAdmin )
		{
			int iMax = m_vFileList.size();
			for(int i2 = 0; i2 < iMax; i2++)
			{
				if(m_vFileList[i2].m_file_path.CompareNoCase(szServerPath) == 0 )
				{
					int iCurSize = atoi( (LPCTSTR) m_vFileList[i2].m_file_size );
					if( iCurSize == iZipFileSize )
					{
						char szError[MAX_PATH]="";
						StringCbPrintf( szError, sizeof( szError ), "Old New File Size Equal : chage File Size : %s", (LPCTSTR)m_vFilePathList[i] );
						//AfxMessageBox( szError );
						LOG.PrintTimeAndLog(0, szError  );
					}
				}
			}
		}

		AddFileListOncheck(szServerPath, dwCRC, iZipFileSize, iUnZipFileSize);

		if( m_eBackUpType == BUT_UNCOMPRESS || m_eBackUpType == BUT_ALL || m_eBackUpType == BUT_UNCOMPRESS_DATE_AND_ONE )
		{
			if(!CopyFileToBackup( m_vFilePathList[i], true, false ) )
			{
				LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
				return false;
			}
		}

		if( m_eBackUpType == BUT_UNCOMPRESS_NO_DATE || m_eBackUpType == BUT_ALL || m_eBackUpType == BUT_UNCOMPRESS_DATE_AND_ONE )
		{
			if(!CopyFileToBackup( m_vFilePathList[i], false, false ) )
			{
				LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
				return false;
			}
		}

		if( m_eBackUpType == BUT_COMPRESS || m_eBackUpType == BUT_ALL )
		{
			if(!CopyFileToBackup( szPacName, true, false ) )
			{
				LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)szPacName);
				return false;
			}
		}

		if(!DeleteFile(szPacName))
		{
			LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szPacName);
			return false;
		}

		if(isFullZip == false)
		{
			if(!DeleteFile(m_vFilePathList[i]))
			{
				LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
				return false;
			}
		}

		InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Upload :"+szServerPath);
		InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
		LOG.PrintTimeAndLog(0, "Completed Upload : %s : %x : %u : %u"
			, szServerPath, dwCRC, iZipFileSize, iUnZipFileSize );
	}
	return true;
}

int CPatchManager::DownProcess()
{
	if(m_vFileList.empty())
		return -1;

	int iMax = m_vFileList.size();
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);

	for(int i =0; i < iMax; i++)
	{
		if(!m_bActive)
			return -2;

		if( !DownloadProcessOneFile( m_vFileList[i].m_file_path ) )
		{
			if( m_vFileList[i].m_file_path.IsEmpty() )
			{
				continue;
			}
			else
			{
				return -1;
			}
		}
		InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
	}
	return 1;
}

bool CPatchManager::DownProcessByUpdateList()
{
	if(m_vUpdateFileList.empty())
		return true;

	int iMax = m_vUpdateFileList.size();
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);

	for(int i =0; i < iMax; i++)
	{
		if(!m_bActive)
			return true;

		if( !DownloadProcessOneFile( m_vUpdateFileList[i] ) )
		{
			return false;
		}
		Sleep(100);
		InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
	}
	return true;
}

bool CPatchManager::PatchInfoUploadProcess( bool bServerFile )
{
	CString szFileName = m_szUploadRoot;
	if( bServerFile )
		szFileName += PATCHINFO_FILE_NAME;
	else
		szFileName += VERSIONPATCH_FILE_NAME;

	if(!PacFile(szFileName))
	{
		LOG.PrintTimeAndLog(0, "팩 실패 : %s", (LPCTSTR)szFileName);
		return false;
	}
	CString szPacName = szFileName;
	szPacName += ".iop";

	DWORD dwCRC     = g_Pac.GetPieceFileCRC( (LPCTSTR)szPacName );
	int   iUnZipFileSize = GetFileSize(szFileName);
	int   iZipFileSize = GetFileSize(szPacName);

	if( m_eBackUpType == BUT_COMPRESS || m_eBackUpType == BUT_ALL || m_eBackUpType == BUT_UNCOMPRESS_NO_DATE )
	{
		if(!CopyFileToBackup( szPacName, true, false ) )
		{
			LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)szPacName);
			return false;
		}
	}

	if( m_eBackUpType == BUT_UNCOMPRESS_DATE_AND_ONE )
	{
		if(!CopyFileToBackup( szPacName, false, true ) )
		{
			LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)szPacName);
			return false;
		}
	}

	if(!UploadFile(szPacName))
	{
		LOG.PrintTimeAndLog(0, "업로드 실패 : %s", (LPCTSTR)szPacName);
		return false;
	}

	/*
	if(!CopyPatchInfoFileToBackUp(szPacName))
	{
		LOG.PrintTimeAndLog(0, "패치 정보 파일 백업 실패 실패 : %s", (LPCTSTR)szPacName);
		return false;
	}
	*/

	if(!DeleteFile(szPacName))
	{
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szPacName);
		return false;
	}
	if(!DeleteFile(szFileName))
	{
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szFileName);
		return false;
	}

	CString szShortName = szFileName;
	szShortName = szShortName.Right(szShortName.GetLength() - (m_szUploadRoot.GetLength()));
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Upload :"+szShortName);

	LOG.PrintTimeAndLog(0, "Completed Upload : %s : %x : %u : %u", szShortName,dwCRC, iZipFileSize, iUnZipFileSize);
	return true;
}

bool CPatchManager::bDownPatchInfo( bool bVersionDown, bool bDelete )
{
	CString szLocalPath = "config/";

	if( bVersionDown )
		szLocalPath += VERSIONPATCH_FILE_NAME;
	else if(bDelete)
		szLocalPath += DELETE_FILE_LIST_NAME;
	else
		szLocalPath += PATCHINFO_FILE_NAME;

	szLocalPath += ".iop";
	if(!DownloadFile(szLocalPath, bVersionDown))
	{
		if( bVersionDown && m_iVersionFileState == VERSION_INFO_SIZE_0 )
		{
			if( IDYES == MessageBox( NULL, "can not checked version file! \nplease check this problem\n- remote file size is 0\n- version file is error\ndo you make new version file?", "WARRING", MB_YESNO ) )
			{
				LOG.PrintTimeAndLog(0, "버전 패치 파일 생성 : %s", (LPCTSTR)szLocalPath);
				m_iVersionFileState = VERSION_INFO_CREATE;
				return true;
			}
		}
		LOG.PrintTimeAndLog(0, "다운로드 실패 : %s", (LPCTSTR)szLocalPath);
		return false;
	}

	return true;
}

//bVersionPatchParse 이값이 true이면 version의 정보도 읽는다.
bool CPatchManager::PatchParse( const char *szPath, bool bVersionPatchParse )
{
	FILE *fp;
	char buffer[MAX_PATH];
	strcpy(buffer, szPath);
	fp = fopen(buffer,"rt");
	if(fp == NULL)
		return false;
	do
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		int bufferSize = strlen(buffer);
		if(buffer[bufferSize-1] == '\n')
			buffer[bufferSize-1] = 0;
		SetPatchListFromFile(buffer ,bVersionPatchParse);

	}while(!feof(fp));
	fclose(fp);

	CString strPath = szPath;
	if(strPath.Find("delete_file_list.cfg") == -1)
	{
		SetFileAttributes(szPath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(szPath);
	}

	return true;
}

void CPatchManager::SetPatchListFromFile( char *pSrc ,bool bVersionPatchParse)
{
	if(pSrc == NULL) return;

	char szFile[MAX_FILE_INFO][MAX_PATH];
	::memset(szFile,0,sizeof(szFile));
	int     len = ::strlen(pSrc);
	int     info_cnt = 0;
	int     info_size= 0;
	char    c = '0';
	for(int i = 0;i < len;i++)
	{
		if(pSrc[i] =='[' || pSrc[i] == ']') continue;
		if(pSrc[i] ==':')
		{
			info_cnt ++;
			info_size=0;
		}
		else
		{
			if(info_cnt >= MAX_FILE_INFO) break;
			if(info_size >= MAX_PATH) break;
			szFile[info_cnt][info_size++] = pSrc[i];
		}
	}

	PATCHINFOFILE hf;	
	hf.m_file_path			= szFile[0];
	hf.m_crc    			= szFile[1];
	hf.m_file_size			= szFile[2];
	hf.m_unzip_file_size	= szFile[3];

	AddFileList(szFile[0],hf, bVersionPatchParse);
}

void CPatchManager::AddFileListOncheck( const CString &rszServerPath, DWORD dwCRC, int iZipFileSize, int iUnZipFileSize )
{
	if( m_iVersionFileState == VERSION_INFO_CREATE )
	{
		PATCHINFOFILE fd;
		fd.m_bExist     = true;
		fd.m_file_path	= rszServerPath;
		fd.m_crc.Format("%x",dwCRC);
		fd.m_file_size.Format("%u",iZipFileSize);
		fd.m_unzip_file_size.Format("%u",iUnZipFileSize);
		m_vVersionFileList.push_back(fd);
	}
	else if( m_iVersionFileState == VERSION_INFO_OK )
	{
		int iMax = m_vVersionFileList.size();
		bool bFind = false;
		for(int i = 0; i < iMax; i++)
		{
			if(m_vVersionFileList[i].m_file_path.CompareNoCase(rszServerPath) == 0)
			{
				PATCHINFOFILE fd;
				m_vVersionFileList[i].m_bExist     = true;
				m_vVersionFileList[i].m_file_path	= rszServerPath;
				m_vVersionFileList[i].m_crc.Format("%x",dwCRC);
				m_vVersionFileList[i].m_file_size.Format("%u",iZipFileSize);
				m_vVersionFileList[i].m_unzip_file_size.Format("%u",iUnZipFileSize);
				bFind = true;
				break;
				//m_vVersionFileList.push_back(fd);
			}
		}
		if( !bFind )
		{
			PATCHINFOFILE fd;
			fd.m_bExist     = true;
			fd.m_file_path	= rszServerPath;
			fd.m_crc.Format("%x",dwCRC);
			fd.m_file_size.Format("%u",iZipFileSize);
			fd.m_unzip_file_size.Format("%u",iUnZipFileSize);
			m_vVersionFileList.push_back(fd);
		}
	}

	//server patch .cfg
	int iMax = m_vFileList.size();
	for(int i = 0; i < iMax; i++)
	{
		if(m_vFileList[i].m_file_path.CompareNoCase(rszServerPath) == 0)
		{
			m_vFileList[i].m_bExist    = true;
			m_vFileList[i].m_file_path = rszServerPath;
			m_vFileList[i].m_crc.Format("%x",dwCRC);
			m_vFileList[i].m_file_size.Format("%u",iZipFileSize);
			m_vFileList[i].m_unzip_file_size.Format("%u",iUnZipFileSize);
			return;
		}
	}
	PATCHINFOFILE fd;
	fd.m_bExist     = true;
	fd.m_file_path	= rszServerPath;
	fd.m_crc.Format("%x",dwCRC);
	fd.m_file_size.Format("%u",iZipFileSize);
	fd.m_unzip_file_size.Format("%u",iUnZipFileSize);
	m_vFileList.push_back(fd);
	return;
}

bool CPatchManager::GetFTPFileInfo( IN const CString &rszFileName,OUT CString &rszServerPath, OUT tm &rTime, OUT int &iSize )
{
	rszServerPath = rszFileName;
	rszServerPath = rszServerPath.Right(rszServerPath.GetLength() - (m_szUploadRoot.GetLength()));

	rszServerPath = m_szPatchRoot+rszServerPath;
	if( ! m_FTP.GetTime(rszServerPath,rTime) )
		return false;

	rszServerPath = rszServerPath.Right( rszServerPath.GetLength() - strlen((LPCTSTR)m_szPatchRoot));
	iSize =  m_FTP.GetLocalFileSize((LPCTSTR)rszFileName);

	return true;
}

bool CPatchManager::UnPacPieceFile( const CString &rszDownPath )
{
	if(g_Pac.UnPacPieceFile(rszDownPath) != CPacUtil::RV_OK)
	{
		CString szHelp = "Fail Pack Piece File UnPacking :";
		szHelp += rszDownPath;
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}

	if( m_eRunState != RS_MAKE_PATCH_PAC ) //옮겨야 백업해야 하기때문에
	{
		SetFileAttributes((LPCTSTR)rszDownPath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)rszDownPath);
	}
	return true;	
}

void CPatchManager::AddFileList( const char *szFileName, const PATCHINFOFILE &rkfd, bool bVersionPatchParse )
{
	char fName[MAX_PATH] = "";
	::_splitpath(szFileName,NULL,NULL,fName,NULL);
	if(::strcmp(fName,"") == 0) return;
	strlwr(fName);
	if(strcmp(fName,"full") == 0)
		CopyPatchInfo(&m_vFileList[0],rkfd);
	else if(strcmp(fName, PATCHINFO_FILE_NAME ) == 0) 
		return;
	else
	{
		if(bVersionPatchParse)
			m_vVersionFileList.push_back(rkfd);
		else
			m_vFileList.push_back(rkfd);
	}
}

bool CPatchManager::IsPutPacFile( CString szClientPath )
{
	szClientPath.MakeLower();

	if(!szClientPath.Find("config/"))
		return true;
	if(!szClientPath.Find("resource/"))
		return true;
	if(!szClientPath.Find("scripts/"))
		return true;
	if(!szClientPath.Find("shader/"))
		return true;
	if(!szClientPath.Find("xml/"))
		return true;
	if(!szClientPath.Find("_map/"))
		return true;
	if(!szClientPath.Find("_map_en/"))
		return true;
	if(!szClientPath.Find("_map_de/"))
		return true;
	if(!szClientPath.Find("_map_fr/"))
		return true;
	if(!szClientPath.Find("_map_it/"))
		return true;
	if(!szClientPath.Find("_map_pl/"))
		return true;
	if(!szClientPath.Find("_map_tr/"))
		return true;

	return false;
}

int CPatchManager::GetFileSize( const CString &rszFileName )
{
	int iFileSize = 0;

	CFile file;
	if(file.Open((LPCTSTR)rszFileName,CFile::modeRead | CFile::shareDenyNone))
	{
		iFileSize = (int) file.GetLength();	
		file.Close();
	}	

	return iFileSize;
}

void CPatchManager::SetUseDisplaySec( bool bUse )
{
	DWORD iSec = 0;
	if(bUse)
		iSec = 1;
	else
		iSec = 0;

	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
			Dlg->SetDisplaySec(iSec);
	}
}

void CPatchManager::SetDisplayTitle( const CString &rszString )
{
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
			Dlg->SetDisplayTitle(rszString);
	}
}

void CPatchManager::GetCurrentDir( OUT CString &rString )
{
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
			rString = Dlg->GetRootDir();
	}
}

void CPatchManager::RunExit()
{
	m_bRunThread = false;
	LOG.CloseAndRelease();
	DirectoryDelete();
	InterfaceManager::GetInstance()->SetUploadBtnEnable(true);
	InterfaceManager::GetInstance()->SetComboBoxEnable(true);
	InterfaceManager::GetInstance()->SetMakeDelFileEnalbe(true);
#ifdef SHIPPING
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(false);
	InterfaceManager::GetInstance()->SetShowLogBtnEnable(false);
	InterfaceManager::GetInstance()->SetDeleteListBtnEnable(false);
	InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(false);
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(true);
	InterfaceManager::GetInstance()->SetPatchInfoFixEnable(false);
	InterfaceManager::GetInstance()->SetUpdateBtnEnable(false);
#else
	InterfaceManager::GetInstance()->SetPatchInfoFixEnable(true);
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(true);

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		InterfaceManager::GetInstance()->SetShowLogBtnEnable(true);
		InterfaceManager::GetInstance()->SetDeleteListBtnEnable(true);
		InterfaceManager::GetInstance()->SetMakeDelFileEnalbe(true);
	}
	else
	{
		InterfaceManager::GetInstance()->SetUpdateBtnEnable(true);
		}

#endif
#if defined(OVERSEAS) //해외 배포용
	CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
	if(Dlg)
	{
		if(!Dlg->GetExportBtnEnableState())
		{
			InterfaceManager::GetInstance()->SetCheckFileSyncEnable(true);
			InterfaceManager::GetInstance()->SetShowLogBtnEnable(true);
			InterfaceManager::GetInstance()->SetDeleteListBtnEnable(true);
			InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(true);
			InterfaceManager::GetInstance()->SetCheckFileSyncEnable(true);
			InterfaceManager::GetInstance()->SetPatchInfoFixEnable(true);
			InterfaceManager::GetInstance()->SetUpdateBtnEnable(true);
			InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(true);
			InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(true);
		}
		else
		{
			InterfaceManager::GetInstance()->SetCheckFileSyncEnable(false);
			InterfaceManager::GetInstance()->SetShowLogBtnEnable(false);
			InterfaceManager::GetInstance()->SetDeleteListBtnEnable(false);
			InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(false);
			InterfaceManager::GetInstance()->SetPatchInfoFixEnable(false);
			InterfaceManager::GetInstance()->SetUpdateBtnEnable(false);
		}
	}
#endif

	SetUseDisplaySec(false);
}

bool CPatchManager::GetFileVesrion( IN const char* szFileName, OUT char* szVersionName, IN int iSize 
	, IN bool isProductVerion /*= false*/ )
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
		delete lpBuff; 
		return false;
	}

	VS_FIXEDFILEINFO *vInfo; 
	UINT uInfoSize; 

	if(0 == VerQueryValue(lpBuff, TEXT("\\"), (LPVOID*)&vInfo, &uInfoSize)) 
	{
		delete lpBuff; 
		return false;
	}

	if(0 == uInfoSize) 
	{
		delete lpBuff; 
		return false; 
	} 

	if(isProductVerion == false)
	{
		sprintf(szVersionName, "%d,%d,%d,%d",HIWORD(vInfo->dwFileVersionMS), LOWORD(vInfo->dwFileVersionMS) 
			,HIWORD(vInfo->dwFileVersionLS) , LOWORD(vInfo->dwFileVersionLS) );
	}
	else
	{
		sprintf(szVersionName, "%d,%d,%d,%d",HIWORD(vInfo->dwProductVersionMS), LOWORD(vInfo->dwProductVersionMS) 
			,HIWORD(vInfo->dwProductVersionLS) , LOWORD(vInfo->dwProductVersionLS) );
	}
	delete lpBuff; 
	return true;
}

bool CPatchManager::IsRightVersion( const CString &rszName )
{
	if( ( rszName.Find("autoupgrade.exe") != -1 ) ||
		( rszName.Find("lostsaga.exe") != -1 )    ||
		( rszName.Find("io3dengine.dll") != -1 ) )
	{
		char szProductName[MAX_PATH]="";
		GetFileVesrion((LPCTSTR)rszName, szProductName, sizeof(szProductName), true);

		if(m_eZoneType == ZONE_TYPE_OFFICE_ADMIN)
		{
			if(strcmp(szProductName, "0,0,0,2") != 0)
				return false;
		}
		else
		{
			if(strcmp(szProductName, "0,0,0,1") != 0)
				return false;
		}
	}

	return true;
}

void CPatchManager::DirectoryDelete()
{
	CString szRoot;
	GetCurrentDir(szRoot);
	//디렉토리 삭제.
	char delName[MAX_PATH] = "";
	sprintf(delName,"%s\\config\\*.*",(LPCTSTR)szRoot);
	SHFILEOPSTRUCT FileOp = {0};	
	FileOp.hwnd = NULL;	
	FileOp.wFunc = FO_DELETE;	
	FileOp.pFrom = delName;	
	FileOp.pTo = NULL;	
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;	
	FileOp.fAnyOperationsAborted = false;	
	FileOp.hNameMappings = NULL;	
	FileOp.lpszProgressTitle = NULL;	
	::SHFileOperation(&FileOp);	
}

void CPatchManager::RunUpload()
{
	FILE * fp = NULL;
	if(fp)
	{
		fclose(fp);
		//szParsePath = szAutoSavePath;
		LOG.PrintTimeAndLog(0, "Load Auto File");
	}

	InitFileList();

	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;


	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if( !bDownPatchInfo(false))
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}
	else if( m_bVersionUp ) 
	{
		if( !bDownPatchInfo( m_bVersionUp ) )
		{
			AfxMessageBox("Fail Down Patch Info!!!!!!!");
			return;
		}
	}

	CString szPacPath = "config/";
	szPacPath += PATCHINFO_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR)szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR)szPacPath ), GetFileSize( (LPCTSTR)szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR)szPacPath )); // unzip size
	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile(szPacPath))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	if(!PatchParse((LPCTSTR)szParsePath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	if( m_bVersionUp && m_iVersionFileState == VERSION_INFO_OK )
	{
		szPacPath = "config/";
		szPacPath += VERSIONPATCH_FILE_NAME;
		szPacPath += ".iop";
		LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR)szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR)szPacPath ), GetFileSize( (LPCTSTR)szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR)szPacPath )); // unzip size

		szUnzipName = szPacPath;
		szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
		SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)szUnzipName);

		if(!UnPacPieceFile(szPacPath))
		{
			AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
			return;
		}

		szParsePath = "config\\";
		szParsePath += VERSIONPATCH_FILE_NAME;
		if(!PatchParse((LPCTSTR)szParsePath, true)) // 읽고 나서 정보 파일 삭제
		{
			AfxMessageBox("Fail Parse Patch Info!!!!!!!");
			return;
		}

	}

	if(!UploadProcess())
	{
		if(m_vFilePathList.empty())
			AfxMessageBox("Empty upload files!!!!!!!!!");
		else
			AfxMessageBox("Fail Upload!!!!!!!!!");
		return;	
	}

	if(!ReConnect())
	{
		if(!ConnectProcess())
		{
			AfxMessageBox("Fail ReConnect!!!!!!!!!");
			return;
		}
	}

	CString szPath = m_szUploadRoot;
	szPath += PATCHINFO_FILE_NAME;

	SetFileAttributes((LPCTSTR)szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szPath);

	if(!WritePatch( szPath, false ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}

	if(!PatchInfoUploadProcess()) 
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}

	if( m_iVersionFileState == VERSION_INFO_OK || m_iVersionFileState == VERSION_INFO_CREATE )
	{
		szPath = m_szUploadRoot;
		szPath += VERSIONPATCH_FILE_NAME;

		SetFileAttributes((LPCTSTR)szPath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)szPath);
	}
	else
		return;

	if(!WritePatch( szPath, false ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}

	if(!PatchInfoUploadProcess( false )) 
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);
	CString szDate;
	szDate.Format("%d/%02d/%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond ); 
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
			Dlg->SaveUploadedDate(szDate);
	}


	LOG.PrintTimeAndLog(0, "Completed Patch.");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Patch.");
}

void CPatchManager::RunCreatePatchInfo()
{
	InitFileList();

	if(!ReConnect())
	{
		if(!ConnectProcess())
		{
			AfxMessageBox("Fail ReConnect!!!!!!!!!");
			return;
		}
	}

	if(!FileListProcess()) 
	{
		AfxMessageBox("Fail FTP List!!!!!!!!!");
		return;	
	}

	int iReturn = DownProcess();
	if( iReturn == -1 )
	{
		AfxMessageBox("Fail Downlaod All Files!!!!!!!!!");
		return;
	}
	else if( iReturn == -2 )
		return;

	CString szPath = m_szUploadRoot;
	szPath += PATCHINFO_FILE_NAME;

	SetFileAttributes((LPCTSTR)szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szPath);

	if( !WritePatch( szPath, false ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}

	if(!PatchInfoUploadProcess()) 
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}

	LOG.PrintTimeAndLog(0, "Completed Patch.");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Patch.");
}

void CPatchManager::RunDeleteList()
{
	InitFileList();

	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	CString szPath = m_szUploadRoot;
	//szPath += "info\\";
	szPath += DELETE_FILE_LIST_NAME;

	if(!PatchParse((LPCTSTR)szPath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	if( m_vFileList.empty() )
	{
		AfxMessageBox("Empty delete file list!!!!!!!");
		return;
	}

	// 서버파일삭제 이거 kh
	const int iMax = m_vFileList.size();
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);
	for (int i = 0; i < iMax ; i++)
	{
		if(m_vFileList[i].m_file_path.IsEmpty())
			continue;

		CString szServerPath = m_szPatchRoot + m_vFileList[i].m_file_path;
		szServerPath += ".iop";
		szServerPath.Replace("\\", "/");

		m_FTP.DeleteFile(szServerPath);	

		InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Server File Delete :"+szServerPath);
		InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
		LOG.PrintTimeAndLog(0, "Completed Server File Delete : %s", szServerPath);
	}

	if( !WritePatch( szPath, false ) ) 
	{
		AfxMessageBox("Fail Write Delete List!!!!!!!!!");
		LOG.PrintTimeAndLog(0, "Fail Write Delete List : %s", (LPCTSTR)szPath);
		return;
	}

	if(!PacFile(szPath))
	{
		AfxMessageBox("Fail Pack!!!!!!!!!");
		LOG.PrintTimeAndLog(0, "팩 실패: %s", (LPCTSTR)szPath);
		return;
	}

	if(!DeleteFile(szPath))
	{
		AfxMessageBox("Fail delete!!!!!!!!!");
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szPath);
		return;
	}

	RunUpload();

	// 서버파일 삭제는 안전한 처리 방안을 고민해야 함.
	// RunUpdatePatchInfo( true );

	LOG.PrintTimeAndLog(0, "Completed Delete File List.");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Delete File List.");
}

void CPatchManager::RunUpdatePatchInfo( bool bCheckExist )
{
	InitFileList();

	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if(!bDownPatchInfo(false))
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}

	CString szPacPath = "config/";
	szPacPath += PATCHINFO_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u"
		, (LPCTSTR) szPacPath
		, g_Pac.GetPieceFileCRC( (LPCTSTR) szPacPath )
		, GetFileSize( (LPCTSTR) szPacPath )
		, g_Pac.GetPieceFileSize( (LPCTSTR) szPacPath )); // unzip size

	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile( szPacPath ))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;
	if(!PatchParse((LPCTSTR)szParsePath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	m_vUpdateFileList.clear();
	if(!FileListProcess()) 
	{
		AfxMessageBox("Fail FTP List!!!!!!!!!");
		return;	
	}

	if( !DownProcessByUpdateList() )
	{
		AfxMessageBox("Fail Download List!!!!!!!!!");
		return;
	}
	m_vUpdateFileList.clear();

	CString szPath = m_szUploadRoot;
	szPath += PATCHINFO_FILE_NAME;

	SetFileAttributes((LPCTSTR)szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szPath);

	if( !WritePatch( szPath, bCheckExist ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}

	if(!PatchInfoUploadProcess()) 
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}

	// no server file announce
	if( m_eRunState == RS_NO_SERVER_FILE_DELETE )
	{
		enum { MAX_ANNOUNCE_FILE = 30, };
		vCString vCStringVec;
		int iCnt = 0;
		int iSize = m_vFileList.size();
		for(int i = 0;i < iSize; i++)
		{
			if( m_vFileList[i].m_bExist )
				continue;

			if( iCnt < MAX_ANNOUNCE_FILE )
				vCStringVec.push_back( m_vFileList[i].m_file_path );
			iCnt++;
		}

		char szAnnounce[MAX_PATH*4]="";
		ZeroMemory( szAnnounce , sizeof( szAnnounce ) );
		StringCbPrintf( szAnnounce, sizeof( szAnnounce ), "No server delete file : %d\n", iCnt );

		for(vCString::iterator iter = vCStringVec.begin(); iter != vCStringVec.end(); ++iter)
		{
			CString &rString = *iter;
			StringCbCat( szAnnounce, sizeof( szAnnounce ), (LPCTSTR) rString );
			StringCbCat( szAnnounce, sizeof( szAnnounce ), "\n" );
		}

		AfxMessageBox(szAnnounce);
	}

	LOG.PrintTimeAndLog(0, "Completed Update Patch Info.");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Update Patch Info.");
}

void CPatchManager::RunInit()
{
	m_bActive = true;
	m_bRunThread = true;
#ifdef SHIPPING
	InterfaceManager::GetInstance()->SetUpdateBtnEnable(true);
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(true);
	InterfaceManager::GetInstance()->SetUploadBtnEnable(true);
#else
	InterfaceManager::GetInstance()->SetUpdateBtnEnable(false);
	InterfaceManager::GetInstance()->SetUploadBtnEnable(false);
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(false);
#endif
	InterfaceManager::GetInstance()->SetMakeFullzipEnable(false);
	InterfaceManager::GetInstance()->SetPatchInfoFixEnable(false);
	InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(false);
	InterfaceManager::GetInstance()->SetShowLogBtnEnable(false);
	InterfaceManager::GetInstance()->SetComboBoxEnable(false);
	InterfaceManager::GetInstance()->SetDeleteListBtnEnable(false);
	InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(false);
	InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(false);
	InterfaceManager::GetInstance()->SetMakeDelFileEnalbe(false);
	

	CString szName;
	InterfaceManager::GetInstance()->GetCurComboBoxText(szName);

	char szCurTime[MAX_PATH]="";
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(szCurTime, "log\\pm[%s]%04d%02d%02d.log", (LPCTSTR)szName, st.wYear, st.wMonth, st.wDay);	
	LOG.OpenLog(0, szCurTime, true);
	LOG.PrintTimeAndLog(0, "[NAME:%s] [IP:%s] [PATH:%s] [ID:%s] [PASSIVE:%d] [Local:%d]"
		, (LPCTSTR)szName, (LPCTSTR)m_szIP, (LPCTSTR)m_szPatchRoot, (LPCTSTR)m_szID, m_bPassive, (int) ioLocalManager::GetLocalType() );
	LOG.PrintTimeAndLog(0, "[BackUp:%s] [Upload:%s]", (LPCTSTR)m_szBackUpRoot, (LPCTSTR)m_szUploadRoot);
	LOG.PrintTimeAndLog(0, "[Sync:%s] [URL:%s]", (LPCTSTR)m_szSyncRoot, (LPCTSTR)m_szPatchURL );

	InterfaceManager::GetInstance()->SetPatchGauge(0,0);

	//버전업 정보 체크 versionup
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
		{
			m_bVersionUp = Dlg->GetVersionEnableState();
			m_iPacDivideSize = Dlg->GetPacDivieSize() * 1024; //MB -> KB
			//m_iPatchType = Dlg->GetPatchTypeState();
		}
	}
}

void CPatchManager::InitFileList()
{
	m_vFileList.clear();
	m_vVersionFileList.clear();
	for (int i = 0; i < 1 ; i++) // full.zip은 리스트에서 맨위 넣기 위한 공간 확보
	{
		PATCHINFOFILE fp;
		m_vFileList.push_back(fp);
	}
}

int CPatchManager::HTTPDownLoadProcess( const CString &szURL, const CString &rszDownloadedFullPath , const CString &rszRootDir, bool bAnnounce )
{
	CString szShortPath = rszDownloadedFullPath;
	szShortPath = szShortPath.Right( szShortPath.GetLength() - rszRootDir.GetLength() );

	if( bAnnounce )
	{
		CString szHelp;
		szHelp = "HTTP Download : ";
		szHelp += szShortPath;
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		SetUseDisplaySec(true);
		SetDisplayTitle(szHelp);
	}


	CString szErrorMsg;
	if( !m_HTTP.OpenUrl(szURL, szErrorMsg) )
	{  

		LOG.PrintTimeAndLog(0, "HTTPDownLoadProcess Error : %s", (LPCTSTR)szErrorMsg);
		m_HTTP.DeleteSession();
		SetUseDisplaySec(false);
		return -1;
	}

	CInternetFile *pIF = m_HTTP.GetInternetFile(); 
	if(!pIF)
	{
		LOG.PrintTimeAndLog(0, "HTTPDownLoadProcess Error : NULL" );
		m_HTTP.DeleteSession();
		SetUseDisplaySec(false);
		return -1;
	}

	// create dir
	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)szShortPath,NULL,dir,NULL,NULL);
	SetCreateDirectory(rszRootDir+"\\", dir);
	//

	SetFileAttributes((LPCTSTR)rszDownloadedFullPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)rszDownloadedFullPath);

	CFile file;
	CFileException ex;

	if(!file.Open((LPCTSTR)rszDownloadedFullPath,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		CString szErrorMsg;
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		szErrorMsg += szError;
		LOG.PrintTimeAndLog(0, "HTTPDownLoadProcess Error : %s", (LPCTSTR)szErrorMsg);

		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		SetUseDisplaySec(false);
		return -1;	
	}

	DWORD write_file_size = 0;
	try
	{
		static char buf[4096]; 
		memset( buf, 0, sizeof(buf) );

		DWORD read_file_cnt = pIF->Read( buf, sizeof(buf) );
		while(read_file_cnt)
		{
			write_file_size += read_file_cnt;
			file.Write((LPCSTR)buf,read_file_cnt);
			memset( buf, 0, sizeof(buf) );
			read_file_cnt = pIF->Read( buf,sizeof(buf) );

			if(!m_bActive) 
			{
				file.Close();
				m_HTTP.DeleteInternetFile();
				m_HTTP.DeleteSession();
				SetUseDisplaySec(false);
				return -2; 
			}
		}

		file.Flush();
		file.Close();
	}
	catch( CFileException *e )
	{
		CString szErrorMsg;

		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "HTTPDownLoadProcess Error : %s", (LPCTSTR)szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		SetUseDisplaySec(false);
		return -1;
	}
	catch( CInternetException *e )
	{
		CString szErrorMsg;
		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "HTTPDownLoadProcess Error : %s", (LPCTSTR)szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		SetUseDisplaySec(false);
		return -1;
	}

	m_HTTP.DeleteInternetFile();
	m_HTTP.DeleteSession();
	SetUseDisplaySec(false);

	return 1;
}



int CPatchManager::IsSameFile( const CString &rszLocalName, const PATCHINFOFILE &rFTPPatchInfo )
{
	// full.zip size
	if( rszLocalName.Find("full.zip") != -1 )
	{
		if( !IsExistFile((LPCTSTR)rszLocalName ) )
			return 0;

		int iFileSize    = GetFileSize(rszLocalName);
		int iFTPFileSize = atoi( (LPCTSTR) rFTPPatchInfo.m_file_size );
		if( iFTPFileSize != iFileSize )
		{
			SetFileAttributes((LPCTSTR)rszLocalName,FILE_ATTRIBUTE_NORMAL);
			::DeleteFile((LPCTSTR)rszLocalName);

			LOG.PrintTimeAndLog(0, "Check Sync:%s, FTP:%u HTTP:%u",(LPCTSTR)rFTPPatchInfo.m_file_path,iFTPFileSize, iFileSize);
			return 0;
		}
		return 1;
	}

	CString szUnZipName = rszLocalName.Left(rszLocalName.GetLength() - 4); // .iop 삭제
	DWORD dwCRC = 0;
	if( IsPutPacFile(rFTPPatchInfo.m_file_path) )
	{
		if( !IsExistFile( (LPCTSTR)rszLocalName ) )
			return 0;

		enum { PASSWORD_TYPE = 1, };
		char szPacPassword[MAX_PASSWORD+1]="";
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );
		dwCRC = g_Pac.GetPieceFileCRC( (LPCTSTR)rszLocalName , szPacPassword );
	}
	else
	{
		if( IsExistFile( (LPCTSTR)szUnZipName ) )
		{
			if(CCrc32Static::FileCrc32Streams((LPCTSTR)szUnZipName, dwCRC) != 0 )
			{
				LOG.PrintTimeAndLog(0, "HttpDownLoopProcess: CRC 실패 : %s", (LPCTSTR)szUnZipName);
				return -1;
			}
		}
		else if( IsExistFile( (LPCTSTR)rszLocalName) )
		{
			SetFileAttributes((LPCTSTR)szUnZipName,FILE_ATTRIBUTE_NORMAL);
			::DeleteFile((LPCTSTR)szUnZipName);

			if(!UnPacPieceFile(rszLocalName))
			{
				LOG.PrintTimeAndLog(0, "HttpDownLoopProcess: %s Fail Unpack : %s", __FUNCTION__, (LPCTSTR)rszLocalName);
				return -1;
			}

			if(CCrc32Static::FileCrc32Streams((LPCTSTR)szUnZipName, dwCRC) != 0 )
			{
				LOG.PrintTimeAndLog(0, "HttpDownLoopProcess: CRC 실패 : %s", (LPCTSTR)szUnZipName);
				return -1;
			}
		}
		else
			return 0;
	}

	char *pTemp      = NULL;
	DWORD dwFTPCRC   =  strtoul((LPCTSTR)rFTPPatchInfo.m_crc, &pTemp, 16);	
	if( dwFTPCRC != dwCRC )
	{
		SetFileAttributes((LPCTSTR)szUnZipName,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)szUnZipName);

		SetFileAttributes((LPCTSTR)rszLocalName,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)rszLocalName);

		LOG.PrintTimeAndLog(0, "Check Sync:%s, FTP:%x : HTTP:%x" ,(LPCTSTR)rFTPPatchInfo.m_file_path, dwFTPCRC, dwCRC);
		return 0;
	}

	return 1;
}

bool CPatchManager::IsExistFile( const char* szFileName )
{
	FILE *pfile;
	if( (pfile = fopen( szFileName, "rt") ) == NULL )
	{
		return false;
	}
	fclose(pfile);
	return true;
}

void CPatchManager::ChangePathLikePathInfo( const CString &rszRootPath, OUT vCString &rvPathList )
{
	if( rvPathList.empty() ) return;

	for(vCString::iterator iter = rvPathList.begin(); iter != rvPathList.end(); ++iter)
	{
		CString &rPath = *iter;
		rPath = rPath.Right( rPath.GetLength() - rszRootPath.GetLength() );
		rPath.Replace( "\\", "/");
	}

	LOG.PrintTimeAndLog(0, "Complete Change Path" );
	InterfaceManager::GetInstance()->SetAnnounceFirstLine( "Complete Change Path" );
}

void CPatchManager::DeleteFilesNoPathInfo( const vCString &rvLocalFileList,const VPATCHINFOFILE &rvPathInfoList, const CString &rszDeleteRoot )
{
	if( rvLocalFileList.empty() ) return;
	if( rvPathInfoList.empty() ) return;

	for(vCString::const_iterator iter = rvLocalFileList.begin(); iter != rvLocalFileList.end(); ++iter)
	{
		const CString &rLocal = *iter;
		bool bPathInfo  = false;
		for(VPATCHINFOFILE::const_iterator iter2 = rvPathInfoList.begin(); iter2 != rvPathInfoList.end(); ++iter2)
		{
			const PATCHINFOFILE &rInfo = *iter2;
			if( rInfo.m_file_path.Find( rLocal ) != -1 )
			{
				bPathInfo = true;
				break;
			}
		}

		if( !bPathInfo )
		{
			CString szFullPath;
			szFullPath = rszDeleteRoot;
			szFullPath += rLocal;
			DeleteFile( szFullPath );
			LOG.PrintTimeAndLog(0, "Delete File %s", (LPCTSTR)rLocal );
			CString Help;
			Help.Format( "Delete File %s", (LPCTSTR)rLocal );
			InterfaceManager::GetInstance()->SetAnnounceFirstLine( Help );
		}
	}

	LOG.PrintTimeAndLog(0, "Complete Delete Files" );
	InterfaceManager::GetInstance()->SetAnnounceFirstLine( "Complete Delete Files" );
}

bool CPatchManager::EncryptFile( const CString &rszFileName )
{
	// read
	CFile file;
	if( !file.Open((LPCTSTR)rszFileName, CFile::modeRead | CFile::shareDenyNone) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Open file.", __FUNCTION__ );
		return false;
	}

	int iSize = (int) file.GetLength();

	char *pData = new char[iSize];
	if( file.Read( pData, iSize ) != iSize )
	{
		delete [] pData;
		file.Close();
		LOG.PrintTimeAndLog( 0, "%s Fail Read file.", __FUNCTION__ );
		return false;
	}

	file.Close();

	// enrypt
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		delete [] pData;
		LOG.PrintTimeAndLog( 0, "%s pLocal == NULL.", __FUNCTION__ );
		return false;
	}

	pLocal->EncryptDecryptData( pData, iSize, pData, iSize, false );

	// save
	DeleteFile( rszFileName );

	if(!file.Open((LPCTSTR)rszFileName,CFile::modeCreate | CFile::modeWrite))
	{
		delete [] pData;
		LOG.PrintTimeAndLog( 0, "%s Fail Open write file.", __FUNCTION__ );
		return false;	
	}
	file.Write( pData, iSize );
	file.Close();  
	delete [] pData;
	return true;
}

bool CPatchManager::DownloadProcessOneFile( const CString &rszFilePath )
{
	if(rszFilePath.IsEmpty())
		return false;

	CString szLocalName = "config/" + rszFilePath;

	bool isFullZip = false;
	if(szLocalName.Find("full.zip") != -1)
		isFullZip = true;


	SetFileAttributes((LPCTSTR)szLocalName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szLocalName);

	// create dir
	char dir[MAX_PATH]	= "";
	char fname[_MAX_FNAME] = "";
	char ext[_MAX_EXT] = "";
	::_splitpath((LPCTSTR)szLocalName,NULL,dir,NULL,NULL);
	CString szRootDir;
	GetCurrentDir(szRootDir);
	SetCreateDirectory(szRootDir+"\\", dir);
	//

	if(!DownloadFile(szLocalName))
	{
		LOG.PrintTimeAndLog(0, "%s 다운 실패 : %s", __FUNCTION__,(LPCTSTR)szLocalName);
		return false;
	}

	// get crc / filesize
	DWORD dwCRC     = 0;
	int   iUnZipFileSize = 0;
	int   iZipFileSize = GetFileSize(szLocalName);

	if(isFullZip == false)
	{
		if(IsPutPacFile(rszFilePath))
		{
			enum { PASSWORD_TYPE = 1, };
			char szPacPassword[MAX_PASSWORD+1]="";
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
				pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );
			iUnZipFileSize = g_Pac.GetPieceFileSize((LPCTSTR)szLocalName, szPacPassword );
			dwCRC = g_Pac.GetPieceFileCRC( (LPCTSTR)szLocalName, szPacPassword );
		}
		else
		{
			iUnZipFileSize = g_Pac.GetPieceFileSize((LPCTSTR)szLocalName );
			CString szUnZipName = szLocalName.Left(szLocalName.GetLength() - 4); // .iop 삭제

			SetFileAttributes((LPCTSTR)szUnZipName,FILE_ATTRIBUTE_NORMAL);
			::DeleteFile((LPCTSTR)szUnZipName);

			if(!UnPacPieceFile(szLocalName))
			{
				LOG.PrintTimeAndLog(0, "%s Fail Unpack : %s", __FUNCTION__, (LPCTSTR)szLocalName);
				return false;
			}

			if(CCrc32Static::FileCrc32Streams((LPCTSTR)szUnZipName, dwCRC) != 0 )
			{
				LOG.PrintTimeAndLog(0, "CRC 실패 : %s", (LPCTSTR)szUnZipName);
				return false;
			}

			if(!DeleteFile(szUnZipName))
			{
				LOG.PrintTimeAndLog(0, "%s 삭제 실패: %s", __FUNCTION__, (LPCTSTR)szLocalName);
				return false;
			}
		}
	}
	//

	AddFileListOncheck(rszFilePath, dwCRC, iZipFileSize, iUnZipFileSize);

	// 다운받은 파일 삭제
	SetFileAttributes((LPCTSTR)szLocalName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szLocalName);

	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Download :"+rszFilePath);
	LOG.PrintTimeAndLog(0, "Completed Downlaod : %s : %x : %u : %u", rszFilePath, dwCRC, iZipFileSize, iUnZipFileSize );

	return true;
}
/********************************************************
server_patch.cfg의 내용을 다운받아 검색 및 수정이 가능합니다.
닫기 버튼을 누를 경우 자동으로 해당 서버에 server_patch.cfg를 업로드 합니다.
*********************************************************/
void CPatchManager::FixServerCfg(bool bDelete)
{
	InitFileList();
	CString szParsePath = "config\\";
	if(!bDelete)
		szParsePath += PATCHINFO_FILE_NAME;
	else
		szParsePath += DELETE_FILE_LIST_NAME;

	bool bNewFile = false;

	//FTP접속 다운로드
	FILE * fp = NULL;
	if(!fp)
	{
		if(!ConnectProcess())  //접속
		{
			AfxMessageBox("Fail Connect!!!!!!!");
			return;
		}

		if(!bDownPatchInfo(false, bDelete))
		{
			if(!bDelete)
			{
				AfxMessageBox("Fail Down Patch Info!!!!!!!");
				return;
			}
			else
			{
				CStdioFile newDeleteFile;
				if(!newDeleteFile.Open(szParsePath, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
				{
					AfxMessageBox("Create Fail !! delete_file_list.cfg");
					return;
				}
				LOG.PrintTimeAndLog(0, "Create : delete_file_list.cfg");
				newDeleteFile.WriteString("Please delete this line.\n");
				newDeleteFile.Close();
				bNewFile = true;
			}
		}

		CString szPacPath = "config/";
		if(!bDelete)
			szPacPath += PATCHINFO_FILE_NAME;
		else
			szPacPath += DELETE_FILE_LIST_NAME;
		szPacPath += ".iop";
		LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR)szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR)szPacPath )
			, GetFileSize( (LPCTSTR)szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR)szPacPath )); // unzip size

		CString szUnzipName = szPacPath;
		szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
		SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
		if(!bNewFile)
			::DeleteFile((LPCTSTR)szUnzipName);

		if(!bNewFile)
		{
			if(!UnPacPieceFile(szPacPath))
			{
				AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
				return;
			}
		}
	}
	else
	{
		fclose(fp);
		LOG.PrintTimeAndLog(0, "Load Auto File");
	}

	if(!ReConnect())
	{
		if(!ConnectProcess())
		{
			AfxMessageBox("Fail ReConnect!!!!!!!!!");
			return;
		}
	}

	if(! RunEditPad(szParsePath, bDelete) )
		return;

	if(!bDelete)
	{
		if(!PacFile(szParsePath))
			LOG.PrintTimeAndLog(0, "팩 실패 : %s", (LPCTSTR)szParsePath);
	}
	
	if(bDelete)
	{
		if(!FixServerCfgforDelete(szParsePath))
		{
			AfxMessageBox("Upload Fail : Server_Patch.cfg");
			return;
		}
	}
	//여기서부터 delete file list 로 체크했으면 server_patch.cfg도 업로드 시키게 하자.
	//우선 server_partch.cfg를 업로드 하고(삭제한 내용으로 수정된) delete_file_list가 만들어져있으니 업로드 함수를 통해 업로드하면 server_patch.cfg에 자동으로 사이즈랑 싹다 기록

	CString szPacName = "";
	if(bDelete)
	{
		CString szPath = "config\\";
		szPath += PATCHINFO_FILE_NAME;
		CStdioFile NewCfg;
		if(NewCfg.Open(szPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			for(size_t i = 0; i < m_vServerPatchStr.size(); ++i)
				NewCfg.WriteString(m_vServerPatchStr[i]);
		}
		NewCfg.Close();		
		szPacName = szPath;
		szParsePath = szPath;
		if(!PacFile(szParsePath))
		{
			LOG.PrintTimeAndLog(0, "팩 실패 : %s", (LPCTSTR)szParsePath);
		}
	}
	else
		szPacName = szParsePath;
	szPacName			+= ".iop";

	DWORD dwCRC				= g_Pac.GetPieceFileCRC( (LPCTSTR)szPacName );
	int   iUnZipFileSize	= GetFileSize(szParsePath);
	int   iZipFileSize		= GetFileSize(szPacName);

	//Backup폴더 생성
	CFileFind CFileFinder;
	if(!CFileFinder.FindFile(m_szBackUpRoot)) //디렉토리가 음슴
		CreateDirectory(m_szBackUpRoot, NULL);


	CString strDate, strToken, strSourDir = szPacName, strMakeDateDir = m_szBackUpRoot, strTempDir = m_szUploadRoot;
	//날짜 생성
	COleDateTime NowDate;
	NowDate = COleDateTime::GetCurrentTime();
	strDate.Format( "%04d%02d%02d", NowDate.GetYear(), NowDate.GetMonth(), NowDate.GetDay());
	strMakeDateDir += strDate;

	//디렉토리 생성
	if(!CFileFinder.FindFile(strMakeDateDir))
		CreateDirectory(strMakeDateDir, NULL);



	//경로 설정
	strMakeDateDir += "\\";

	strMakeDateDir += PATCHINFO_FILE_NAME;
	strTempDir += PATCHINFO_FILE_NAME;

	strTempDir += ".iop";

	CopyFile(strSourDir, strTempDir, FALSE); //업로드 경로 맞추기용
	strSourDir = strSourDir.Left(strSourDir.GetAllocLength() - 4); //.iop삭제
	CopyFile(strSourDir, strMakeDateDir, FALSE); //백업



	if(!UploadFile(strTempDir))
	{
		LOG.PrintTimeAndLog(0, "업로드 실패 : %s", (LPCTSTR)strTempDir);
	}
	if(!DeleteFile(strTempDir))
	{
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)strTempDir);
	}
	if(!DeleteFile(szPacName))
	{
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szPacName);
	}
	if(!DeleteFile(szParsePath))
	{
		LOG.PrintTimeAndLog(0, "삭제 실패 : %s", (LPCTSTR)szParsePath);
	}

	//업로드 출력
	CString szShortName = strTempDir;
	szShortName = szShortName.Right(szShortName.GetLength() - (m_szUploadRoot.GetLength()));
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Upload :"+szShortName);
	LOG.PrintTimeAndLog(0, "Completed Upload : %s : %x : %u : %u", szShortName,dwCRC, iZipFileSize, iUnZipFileSize);

	SYSTEMTIME st;
	GetLocalTime(&st);
	CString szDate;
	szDate.Format("%d/%02d/%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond ); 
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
			Dlg->SaveUploadedDate(szDate);
	}
	//print Log
	LOG.PrintTimeAndLog(0, "ServerPatch.cfg Upload Completed.");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("ServerPatch.cfg Upload Completed.");

	if(!::MoveFileEx("config/delete_file_list.cfg",m_szUploadRoot+"delete_file_list.cfg", MOVEFILE_REPLACE_EXISTING))
		MessageBox(NULL,"파일이 존재하지 않습니다.","Error",MB_OK);
	else		
		RunUpload();
}

bool CPatchManager::RunEditPad( const CString& strPath, bool bDelete )
{
	CFileFind	CFileFinder;

	CString szFileName;
	if(bDelete)
		szFileName = DELETE_FILE_LIST_NAME;
	else
		szFileName = PATCHINFO_FILE_NAME;

	

	if(!CFileFinder.FindFile(strPath)) //해당 파일이 없음
	{
		szFileName += "파일이 해당 경로에 없습니다.";
		TRACE(szFileName);
		return false;
	}
	CEditDlg	Editdlg;
	Editdlg.SetLoadFilePath(strPath);
	int test = Editdlg.DoModal();
	Editdlg.DestroyWindow();

	szFileName = "Oops! +_+\n"+szFileName;
	szFileName += "파일을 업데이트할까요??";
	if( AfxMessageBox(szFileName,MB_YESNO) == IDNO)
	{
		return false;
	}

	CStdioFile NewCfg;
	if(NewCfg.Open( strPath , CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		NewCfg.WriteString(*(Editdlg.GetEditString()));
	return true;
}

CPatchManager::ReturnState CPatchManager::DownLoadProcess( const CString &szURL, int iRemoteFileSize, const CString &rszDownloadedFullPath )
{
	int iCnt	= 0;
	while(true)
	{
		CString szErrorMsg;
		if( !m_HTTP.OpenUrl(szURL, szErrorMsg) )
		{
			// 			if(iCnt >= 2) //카운트 제거 무제한 시도
			// 			{
			// 				if( IDRETRY == MessageBox( GetDesktopWindow(), "이런 다운로드가 실패했습니다. -_-;", "Server Busy", MB_RETRYCANCEL | MB_ICONWARNING | MB_TOPMOST) )
			// 					continue;
			// 				else
			// 					return RETURN_ERROR;
			// 			}
			m_HTTP.DeleteSession();
			++iCnt;
			continue;
		}
		else
			break;
	}
	CInternetFile *pIF = m_HTTP.GetInternetFile(); 
	int iSize = pIF->GetLength(); //size를 원격에서 파악해보자~??
	if(!pIF)
	{
		m_HTTP.DeleteSession();
		MessageBox(NULL, "pIF Pointer failed", "Error", MB_OK);
		return RETURN_ERROR;
	}

	CFile file;
	CFileException ex;

	if(!file.Open((LPCTSTR)rszDownloadedFullPath,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;	
	}
	if( iRemoteFileSize != 0 )
	{
	}


	DWORD write_file_size = 0;
	try
	{
		static char buf[4096]; 
		ZeroMemory(buf , sizeof(buf));

		DWORD read_file_cnt = pIF->Read( buf, sizeof(buf) ); //download size
		while(read_file_cnt)
		{
			write_file_size += read_file_cnt;
			file.Write((LPCSTR)buf,read_file_cnt);

			memset( buf, 0, sizeof(buf) );
			read_file_cnt = pIF->Read( buf,sizeof(buf) );

			if(!m_bActive) 
			{
				file.Close();
				m_HTTP.DeleteInternetFile();
				m_HTTP.DeleteSession();
				return RETURN_STOP;
			}
		}
		file.Flush();
		file.Close();
	}
	catch( CFileException *e )
	{
		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			e->Delete();
		}
		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;
	}
	catch( CInternetException *e )
	{
		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			e->Delete();
		}
		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;
	}

	m_HTTP.DeleteInternetFile();
	m_HTTP.DeleteSession();
	LOG.PrintTimeAndLog(1, "Complete - download - url:%s - path:%s", (LPCTSTR)szURL, (LPCTSTR)rszDownloadedFullPath);
	return RETURN_NEXT;
}

bool CPatchManager::DeleteFileRepeat( const CString &strPath )
{
	enum { MAX_FILE_CONTROL_LOOP = 100,	};

	for(int i = 0 ; i < MAX_FILE_CONTROL_LOOP ; i++) 
	{
		DWORD dwReturn = GetFileAttributes(strPath);
		if( dwReturn == FILE_ATTRIBUTE_DIRECTORY)
			return true;
		if( dwReturn == INVALID_FILE_ATTRIBUTES) // 파일 없다. 
			return true;
		if( DeleteFile(strPath) != 0) // 성공 
			return true;

		SetFileAttributes( strPath, FILE_ATTRIBUTE_ARCHIVE ); // 보통 속성으로 변경 
		Sleep(1);
	}
	return false;
}

void CPatchManager::SetCreateDirectoryByFullPath( const CString &strDirPath )
{
	if(strDirPath.IsEmpty()) return;
	CString strDir, strPath = strDirPath;
	if(strDirPath.Right(1) != '\\') //파일명이 붙어있으면 지운다.
	{
		strPath.MakeReverse();
		AfxExtractSubString( strDir, strPath, 0,'\\');
		strPath.MakeReverse();
		strPath = strPath.Left( strPath.GetLength() - strDir.GetLength());
		strDir = "";
	}
	for(int i = 0 ; i < strPath.GetLength() ; ++i)
	{
		strDir = strPath.Left(i+1);
		if(strDir.Right(1) == '\\')
		{
			if( access(strDir , 0) == -1)
				CreateDirectory(strDir, NULL);
		}
	}
}

void CPatchManager::SetSyncKeynURL( CString& strKey, const CString& strPath )
{
	m_szServerType	= strKey;
	m_szSyncLiveUrl	= strPath;
}

/*********************************************************
server_patch.cfg를 다운로드 받고 해당 crc정보를 로드 합니다.
그리고 해당 경로 URL을 통해 파일을 다운로드 한 후 UnPac 후 CRC정보를 비교하여
내용을 국가명__FileSyncReport.txt에 기록합니다.
*********************************************************/
void CPatchManager::CheckLiveFileSync()
{
	CString strDate;
	COleDateTime NowDate;
	NowDate = COleDateTime::GetCurrentTime();
	strDate.Format( "%04d-%02d-%02d_%02d%02d%02d"
		, NowDate.GetYear(), NowDate.GetMonth(), NowDate.GetDay()
		, NowDate.GetHour(), NowDate.GetMinute(), NowDate.GetSecond() );
	m_szServerType += "_";
	m_szServerType += strDate;
	m_szServerType += "_FileSyncReport";
	DeleteFile(m_szServerType);
	//get ipaddress
	//GetMyIPAddr();


	std::ofstream FileReportText;
	FileReportText.open( m_szServerType, std::ios_base::out | std::ios_base::app );
	FileReportText << m_szSyncLiveUrl << "에 대한 리포트 입니다.\n결과 : 파일명 [ SERVERPATCH_CRC / DOWNLOADFILE CRC ]" << endl;

	InitFileList();
	CString szURL = m_szSyncLiveUrl;
	szURL += "server_patch.cfg.iop";

	char szDir[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, szDir);
	CString szDownloadedFullPath = szDir;
	szDownloadedFullPath += "\\";
	szDownloadedFullPath +=	"config\\server_patch.cfg.iop";
	DWORD dwDownloadedFileSize = 0;

	switch(DownLoadProcess(szURL, 0, szDownloadedFullPath))
	{
	case RETURN_ERROR:
	case RETURN_STOP:
		MessageBox(NULL, szDownloadedFullPath, "Download Error", MB_OK);
		return;
		break;
	}
	if(!UnPacPieceFile(szDownloadedFullPath))
		MessageBox(NULL, szDownloadedFullPath, "UnPac Error", MB_OK);

	if(!PatchParse( szDownloadedFullPath.Left( szDownloadedFullPath.GetLength() - 4)) ) // 읽고 나서 정보 파일 삭제
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");

	int iMax = m_vFileList.size();
	int iErrCnt = 0;
	FileReportText << "검사할 파일의 갯수 : " << iMax << endl;
	CString strDownloadFile;
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);
	for(int i = 0 ; i < m_vFileList.size() ; ++i) //0번 full.zip은 스킵합시당~
	{
		FileReportText.open( m_szServerType, std::ios_base::out | std::ios_base::app );
		szURL = m_szSyncLiveUrl;
		szDownloadedFullPath = szDir;
		szDownloadedFullPath += "\\config\\";
		strDownloadFile = m_vFileList[i].m_file_path;
		szURL += strDownloadFile;
		szDownloadedFullPath += strDownloadFile;
		szDownloadedFullPath.Replace("/","\\");
		SetCreateDirectoryByFullPath( szDownloadedFullPath );
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(CString("Download : ") + strDownloadFile);
		if( RETURN_NEXT != DownLoadProcess(szURL, _ttoi(m_vFileList[i].m_file_size) , szDownloadedFullPath ))
		{
			FileReportText << "[ERR]Down : " << szURL << endl;
			++iErrCnt;
			continue;
		}
		char szComment[MAX_PATH]="";
		g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)szDownloadedFullPath );
		char szPacPassword[MAX_PATH]="";
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );
		if( CPacUtil::ReturnValue::RV_OK !=  g_Pac.UnPacPieceFileWithPassword((LPCTSTR)szDownloadedFullPath, szPacPassword ))
		{
			FileReportText << "[ERR]UnPAC : " << m_vFileList[i].m_file_path << endl;
			continue;
		}
		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)szDownloadedFullPath.Left( szDownloadedFullPath.GetLength() - 4), dwCRC);
		char* stop;
		DWORD dwServerPatchCRC = strtoul(m_vFileList[i].m_crc, &stop,16); //진수 변환
		if( dwServerPatchCRC != dwCRC )
		{
			FileReportText << "[ERR]CRC : " << m_vFileList[i].m_file_path << " [ " << dwServerPatchCRC <<" | "<< dwCRC << " ]" << endl;
		}
		else
		{
			FileReportText << "[OK] : " << m_vFileList[i].m_file_path << "[ " << dwServerPatchCRC << " | "<<dwCRC << " ]" << endl;
			DeleteFile(szDownloadedFullPath);
			DeleteFile(szDownloadedFullPath.Left( szDownloadedFullPath.GetLength() - 4));
		}
		InterfaceManager::GetInstance()->SetPatchGauge( iMax, i +1 );
		FileReportText.close();
	}
	FileReportText.open( m_szServerType, std::ios_base::out | std::ios_base::app );
	FileReportText << "All Work Complete! :-)" << endl;
	FileReportText.close();
	if( iErrCnt == 0)
	{
		MessageBox(NULL,"All Work Complete! :-)", "SUCCESS", MB_OK);
#ifdef SHIPPING
		if(SendSyncFile())
			DeleteFile(m_szServerType);
#endif
	}
	else
		MessageBox(NULL,"File Check Error\n(Error File Save the config folder)", "FAIL", MB_OK);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("File Check complete");
}

void CPatchManager::RunUpdateSelectFile()
{
	//server_patch.cfg 다운로드
	InitFileList();
	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if(!bDownPatchInfo(false))
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}

	CString szPacPath = "config/";
	szPacPath += PATCHINFO_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR) szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR) szPacPath ), GetFileSize( (LPCTSTR) szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR) szPacPath )); // unzip size

	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile( szPacPath ))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;
	if(!PatchParse((LPCTSTR)szParsePath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
		{
			std::vector<S_PATCHINFO> &vecPatchFileInfo = Dlg->GetPatchFileInfoList();
			bool bFTPUp = true;
			if( Dlg->GetZipfileState() )
				bFTPUp = false;
			if( !vecPatchFileInfo.empty())
			{
				for( int i = 0 ; i < vecPatchFileInfo.size() ; ++i )
				{
					bool bIOPFile = false;
					char szPacPassword[MAX_PASSWORD+1]="";
					char szPacComment[MAX_PATH]="";
					//if( IsPutPacFile( szShortPath ))
					{
						enum { PASSWORD_TYPE = 1, };
						ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
						if( pLocal )
							pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );

						StringCbPrintf( szPacComment, sizeof( szPacComment ), "%d", PASSWORD_TYPE );
					}
					if( -1 == vecPatchFileInfo[i].m_filePath.Find( ".iop"))
					{
						//서버파일 & 툴 파일 일 경우 crc 갱신 제외
						if( -1 != vecPatchFileInfo[i].m_filePath.Find("ServerPatch") || -1 != vecPatchFileInfo[i].m_filePath.Find("ToolPatch") )
						{
							continue;
						}

						if(!PacFile(vecPatchFileInfo[i].m_filePath, szPacPassword, szPacComment ) )
						{
							LOG.PrintTimeAndLog(0, "Pac Fail : %s", (LPCTSTR)vecPatchFileInfo[i].m_filePath);
						}	
					}
					else
						bIOPFile = true;

					DWORD dwCRC = 0;
					CString strPatchFilePath = vecPatchFileInfo[i].m_filePath;
					if( !bIOPFile )
						strPatchFilePath += ".iop";
					dwCRC = g_Pac.GetPieceFileCRC( strPatchFilePath, szPacPassword );
					//CCrc32Static::FileCrc32Streams( vecPatchFileInfo[i].m_filePath, dwCRC); //iop가 없음
					CString strFileName = vecPatchFileInfo[i].m_fileName;
					CString strFilePath;
					if(!bFTPUp) //Zip파일인 경우
					{
						strFilePath = vecPatchFileInfo[i].m_filePath.Right( ( (vecPatchFileInfo[i].m_filePath.GetLength() - vecPatchFileInfo[i].m_filePath.Find("ClientPatch"))-19) );
						strFilePath.Replace("\\","/");
					}
					strFileName += ".iop";
					bool bfalse = false;
					bool bFind = false;
					for( int j = 0 ; j < m_vFileList.size() ; ++j, bfalse = false )
					{
						CString strPatchListFileName;
						//비교할 파일명 자르기
						if( bFTPUp )
						{
							if( !bfalse && m_vFileList[j].m_file_path.Find('/') != -1 )
							{
								bfalse = true;
								strPatchListFileName = m_vFileList[j].m_file_path.Right(m_vFileList[j].m_file_path.GetLength() - m_vFileList[j].m_file_path.ReverseFind('/') -1);
							}
							else if(!bfalse)//"config/sp2_not_make_id_it.ini.iop" //이름 순으로 나열되어 있기 때문에
							{
								strPatchListFileName = m_vFileList[j].m_file_path;
							}
							else
								strPatchListFileName = m_vFileList[j].m_file_path.Right(m_vFileList[j].m_file_path.GetLength() - m_vFileList[j].m_file_path.ReverseFind('/') -1);
						}
						else if( !bFTPUp && !strFilePath.IsEmpty() )
						{
							strPatchListFileName = m_vFileList[j].m_file_path;
							strFileName = strFilePath;
						}

						//if( strcmp( strPatchListFileName.MakeUpper(), strFilePath.MakeUpper()) == 0 ) //경로로 비교
						if( strcmp( strPatchListFileName.MakeUpper(), strFileName.MakeUpper()) == 0 )
						{
							m_vFileList[j].m_file_size.Format("%u",GetFileSize( strPatchFilePath ));

							//unzip 사이즈 구하기
							if( bIOPFile )
							{
								m_vFileList[j].m_unzip_file_size.Format("%u", g_Pac.GetPieceFileSize( strPatchFilePath,szPacPassword ) ); //unzip 사이즈 구하기
							}
							else
								m_vFileList[j].m_unzip_file_size.Format("%u", GetFileSize( vecPatchFileInfo[i].m_filePath));

							m_vFileList[j].m_crc.Format("%x", dwCRC);
							LOG.PrintTimeAndLog(0, "%s File Crc change - %s [ %lu ]", strFileName.MakeLower(), __FUNCTION__, dwCRC);
							bFind = true;
							break;
						}
					}

					//server_patch.cfg에서 파일의 경로를 찾지 못한 경우
					if( !bFind )
					{
						CString strTmp;
						if( bFTPUp )
						{
							if( vecPatchFileInfo[i].m_filePath.Find(".dds") != -1 )
							{
								strTmp = vecPatchFileInfo[i].m_filePath;
								vecPatchFileInfo[i].m_filePath = "_map/resource/texture/";
								vecPatchFileInfo[i].m_filePath += vecPatchFileInfo[i].m_fileName;
							}
							else if( vecPatchFileInfo[i].m_filePath.Find(".txt") != -1 )
							{
								strTmp = vecPatchFileInfo[i].m_filePath;
								vecPatchFileInfo[i].m_filePath = "_map/resource/text/";
								vecPatchFileInfo[i].m_filePath += vecPatchFileInfo[i].m_fileName;
							}
						}

						PATCHINFOFILE pf; //신규 파일 저장
						pf.m_file_path = vecPatchFileInfo[i].m_filePath;
						if( -1 == vecPatchFileInfo[i].m_filePath.Find(".iop") )
							pf.m_file_path += ".iop";
						//pf.m_file_path.Replace("\\","/"); //경로 텍스트 수정
						pf.m_bExist = false;
						if( bFTPUp )
						{
							if( !strTmp.IsEmpty() )
								vecPatchFileInfo[i].m_filePath = strTmp;
							if( -1 == vecPatchFileInfo[i].m_filePath.Find(".iop"))
								vecPatchFileInfo[i].m_filePath += ".iop";
						}

						pf.m_file_size.Format("%u",GetFileSize( vecPatchFileInfo[i].m_filePath));
						if( -1 != vecPatchFileInfo[i].m_filePath.Find(".iop"))
							pf.m_unzip_file_size.Format("%u",  g_Pac.GetPieceFileSize( vecPatchFileInfo[i].m_filePath ,szPacPassword ) );
						else
							pf.m_unzip_file_size.Format("%u", GetFileSize( vecPatchFileInfo[i].m_filePath.Left( vecPatchFileInfo[i].m_filePath.GetLength() -4)));
						pf.m_crc.Format("%x", dwCRC);
						m_vFileList.push_back(pf);
						char szPrint[MAX_PATH] = "";
						strFileName.MakeLower();
						StringCbPrintf(szPrint, sizeof(szPrint), "Add New Pac File : %s", strFileName );
						InterfaceManager::GetInstance()->SetAnnounceFirstLine( szPrint );
						LOG.PrintTimeAndLog(0, "%s - New File Add -[%s:%x:%lu:%lu]", __FUNCTION__, strFileName, dwCRC, pf.m_file_size, pf.m_unzip_file_size );
					}
				}
			}

			CString strOldUploaRoot = "";
			if(bFTPUp && (vecPatchFileInfo.size() != 0) )
			{
				if(!IsConnect()) //ftp체크
					ConnectProcess();		
			}

			for( int i = 0 ; i < vecPatchFileInfo.size() ; ++i)
			{
				if( vecPatchFileInfo[i].m_fileName == "" )
					continue;
				if( bFTPUp )
				{
					if( -1 == vecPatchFileInfo[i].m_filePath.Find(".iop"))
						vecPatchFileInfo[i].m_filePath += ".iop";
					UploadIOP(vecPatchFileInfo[i].m_filePath, true); //fileupdate
				}
				else
				{
					//파일 복사 및 삭제
					if(!FileCopyAndDelete(vecPatchFileInfo[i].m_filePath, vecPatchFileInfo[i].m_file_patch_path, true ))
					{
						LOG.PrintTimeAndLog(0, "%s FileCopyAndDelete ERROR -%s", vecPatchFileInfo[i].m_filePath , __FUNCTION__);
						char szText[MAX_PATH] = "";
						StringCbPrintf(szText,sizeof(szText),"ERROR File Copy %s", vecPatchFileInfo[i].m_filePath);
						InterfaceManager::GetInstance()->SetAnnounceFirstLine(szText);
						continue;;
					}
					if( !Dlg->GetZipfileState() )
					{
						//파일 백업
						if( !CopyFileToBackup( vecPatchFileInfo[i].m_filePath +".iop", true ))
						{
							LOG.PrintTimeAndLog(0, "복사 실패 : %s", (LPCTSTR)vecPatchFileInfo[i].m_filePath );
							char szText[MAX_PATH] = "";
							StringCbPrintf(szText,sizeof(szText),"ERROR File BackUp Error %s", vecPatchFileInfo[i].m_filePath);
							InterfaceManager::GetInstance()->SetAnnounceFirstLine(szText);
							return;
						}
					}
				}
				DeleteFile( vecPatchFileInfo[i].m_filePath );
				DeleteFile( vecPatchFileInfo[i].m_filePath +".iop" );
			}
			if( !strOldUploaRoot.IsEmpty() )
				m_szUploadRoot = strOldUploaRoot;
		}
	}

	if(!WritePatch( m_szUploadRoot + PATCHINFO_FILE_NAME, false ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}

	if(!PatchInfoUploadProcess())
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("File Update Complete!");
}

//Client 이하 폴더를 생성합니다.
void CPatchManager::CreatePatchFolder( const CString& szPathDest )
{
	CFileFind cFileFinder;  
	CString strFinePath = szPathDest;
	strFinePath.Replace('\\','/');
	strFinePath.MakeLower();
	int iPoint = strFinePath.Find("client/");
	if( iPoint <= 0 )
		return;
	CString strCheckPath;
	CString strSearchPath = strFinePath.Left( iPoint + 7);
	CString strMakeFolder = strFinePath.Right( strFinePath.GetLength() - strSearchPath.GetLength());
	strMakeFolder = strMakeFolder.Left( strMakeFolder.ReverseFind('/'));
	SetCreateDirectory(strSearchPath, strMakeFolder);
	return;
}

bool CPatchManager::FileCopyAndDelete( const CString &szPath, const CString& szPathDest, bool bMakeFolder )
{
	if( szPathDest.IsEmpty())
	{
		char szErr[MAX_PATH] = "";
		StringCbPrintf(szErr,sizeof(szErr), "Sorry! Can Not Found File Path\n%s",szPath.Right( szPath.GetLength() - (szPath.ReverseFind('\\') + 1) ));
		MessageBox(NULL,szErr,"ERROR",MB_OK);
		LOG.PrintTimeAndLog(0, "Can not found File Path %s - %s", szPath, __FUNCTION__);
		return false;
	}

	CString strIOP = szPath;
	if( -1 != strIOP.Find("serverpatch") && -1 != strIOP.Find("toolpatch"))
	{
		bool bIOPDelete = false;
		if( -1 == szPath.Find(".iop"))
		{
			bIOPDelete = true;
			strIOP = szPath + ".iop";
		}
	}

	if( bMakeFolder )
	{
		CreatePatchFolder( szPathDest );
	}

	if( CopyFile(strIOP , szPathDest, false)) //덮어쓰기
	{
		char szPath[MAX_PATH] = "";
		::GetCurrentDirectory( MAX_PATH, szPath );
		char szText[MAX_PATH] = "";
		StringCbPrintf(szText,sizeof(szText), "CopySuccess:%s", szPathDest.Right( szPathDest.GetLength() - strlen(szPath)));
		InterfaceManager::GetInstance()->SetAnnounceFirstLine( szText );
		//		StringCbPrintf(szText,sizeof(szText), "Sour : %s", strIOP.Right( strIOP.GetLength() - strlen(szPath)) );
		//		InterfaceManager::GetInstance()->SetAnnounceFirstLine( szText );
		// 		char szPrint[MAX_PATH]= "";
		// 		StringCbPrintf(szPrint, sizeof(szPrint), "File Copy Success - %s", szPathDest);
		// 		InterfaceManager::GetInstance()->SetAnnounceFirstLine( szPrint );
		Sleep(500);
	}
	else
	{
		char szPrint[MAX_PATH]= "";
		DWORD dwCode = GetLastError();
		char szPath[MAX_PATH] = "";
		::GetCurrentDirectory( MAX_PATH, szPath );
		char szText[MAX_PATH] = "";
		StringCbPrintf(szText,sizeof(szText), "CopyFail(%lu):%s", GetLastError() ,szPathDest.Right( szPathDest.GetLength() - strlen(szPath)));
		//StringCbPrintf(szPrint, sizeof(szPrint), "File Copy Fail GetLastErr(%lu)", dwCode);
		//LOG.PrintTimeAndLog(0, "%s - File Copy Fail- %s" __FUNCTION__ , szPathDest);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine( szText );
		return false;
	}
	return true;
}

void CPatchManager::RunFolderFileUpdate()
{
	InitFileList();
	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if(!bDownPatchInfo(false))
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}

	CString szPacPath = "config/";
	szPacPath += PATCHINFO_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR) szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR) szPacPath ), GetFileSize( (LPCTSTR) szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR) szPacPath )); // unzip size

	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile( szPacPath ))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;
	if(!PatchParse((LPCTSTR)szParsePath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	if(AfxGetApp())
	{
		CAutoUpgradeDlg *Dlg = static_cast<CAutoUpgradeDlg*> ( AfxGetApp()->GetMainWnd() );
		if(Dlg)
		{
			std::vector<S_PATCHINFO> &vecPatchFileInfo = Dlg->GetPatchFileInfoList();
			bool bFTPUp = true;
			if( !vecPatchFileInfo.empty())
			{
				for( int i = 0 ; i < vecPatchFileInfo.size() ; ++i )
				{
					char szComment[MAX_PATH]="";
					g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)vecPatchFileInfo[i].m_filePath );
					char szPacPassword[MAX_PATH]="";
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if( pLocal )
						pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );
					if( CPacUtil::ReturnValue::RV_OK !=  g_Pac.UnPacPieceFileWithPassword((LPCTSTR)vecPatchFileInfo[i].m_filePath, szPacPassword ))
					{
						LOG.PrintTimeAndLog(0,"%s UnPacError %s", __FUNCTION__, vecPatchFileInfo[i].m_filePath );
						char szText[MAX_PATH] = "";
						StringCbPrintf(szText,sizeof(szText), "Oops! UnPac Error. Plz check for iop file - %s", vecPatchFileInfo[i].m_filePath);
						InterfaceManager::GetInstance()->SetAnnounceFirstLine(szText);
						return;
					}
					DWORD dwCRC = 0;
					CString strFilePath = vecPatchFileInfo[i].m_filePath.Right( ( (vecPatchFileInfo[i].m_filePath.GetLength() - vecPatchFileInfo[i].m_filePath.Find("Client\\")) - 7 ));
					strFilePath.Replace("\\","/");
					dwCRC = g_Pac.GetPieceFileCRC( vecPatchFileInfo[i].m_filePath, szPacPassword );
					CString strFileName = vecPatchFileInfo[i].m_fileName + ".iop";
					char* stop;
					bool bfalse = false;
					bool bFind = false;
					for( int j = 0 ; j < m_vFileList.size() ; ++j, bfalse = false )
					{
						CString strPatchListFileName;
						strPatchListFileName = m_vFileList[j].m_file_path;
						if( strcmp( strPatchListFileName.MakeUpper(), strFilePath.MakeUpper()) == 0 ) 
						{
							bFind = true;
							DWORD dwServerPatchCRC = strtoul(m_vFileList[j].m_crc, &stop,16); //진수 변환
							if( dwServerPatchCRC != dwCRC)
							{
								//GetFileSize
								m_vFileList[j].m_file_size.Format("%u",GetFileSize( vecPatchFileInfo[i].m_filePath));
								m_vFileList[j].m_unzip_file_size.Format("%u", GetFileSize( vecPatchFileInfo[i].m_filePath.Left( vecPatchFileInfo[i].m_filePath.GetLength() -4)));
								m_vFileList[j].m_crc.Format("%x", dwCRC);
								LOG.PrintTimeAndLog(0, "%s File Crc change - %s [ %lu -> %lu ]", strFileName, __FUNCTION__, dwServerPatchCRC, dwCRC);
								break;
							}
							else
							{
								LOG.PrintTimeAndLog(0, "%s is Same File - %s", strFileName, __FUNCTION__);
								break;
							}
						}
					}
					if( !bFind )
					{
						PATCHINFOFILE pf; //신규 파일 저장
						pf.m_file_path = vecPatchFileInfo[i].m_filePath.Right( ( (vecPatchFileInfo[i].m_filePath.GetLength() - vecPatchFileInfo[i].m_filePath.Find("Client\\")) - 7 ));
						pf.m_file_path.Replace("\\","/"); //경로 텍스트 수정
						pf.m_bExist = false;
						pf.m_file_size.Format("%u",GetFileSize( vecPatchFileInfo[i].m_filePath));
						pf.m_unzip_file_size.Format("%u", GetFileSize( vecPatchFileInfo[i].m_filePath.Left( vecPatchFileInfo[i].m_filePath.GetLength() -4)));
						pf.m_crc.Format("%x", dwCRC);
						m_vFileList.push_back(pf);
						char szPrint[MAX_PATH] = "";
						StringCbPrintf(szPrint, sizeof(szPrint), "Add New Pac File : %s", pf.m_file_path );
						InterfaceManager::GetInstance()->SetAnnounceFirstLine( szPrint );
						LOG.PrintTimeAndLog(0, "%s - New File Add -[%s:%x:%lu:%lu]", __FUNCTION__, strFileName, dwCRC, pf.m_file_size, pf.m_unzip_file_size );
					}
				}
			}

			CString strOldUploaRoot = "";
			if(bFTPUp && (vecPatchFileInfo.size() != 0) )
			{
				if(!IsConnect()) //ftp체크
					ConnectProcess();		
				strOldUploaRoot = m_szUploadRoot;
				CString strPath = vecPatchFileInfo[0].m_filePath;
				int iPoint = strPath.Find("\\Client\\");
				CString strCheckPath;
				m_szUploadRoot = strPath.Left( iPoint + 8);
			}

			for( int i = 0 ; i < vecPatchFileInfo.size() ; ++i)
			{
				if( bFTPUp )
				{
					UploadIOP(vecPatchFileInfo[i].m_filePath); //fileupdate
				}
				else
				{
					//PatchPath 복사 및 삭제
					if(!FileCopyAndDelete(vecPatchFileInfo[i].m_filePath, vecPatchFileInfo[i].m_file_patch_path, true ))
					{
						LOG.PrintTimeAndLog(0, "%s FileCopyAndDelete ERROR -%s", vecPatchFileInfo[i].m_filePath , __FUNCTION__);
						char szText[MAX_PATH] = "";
						StringCbPrintf(szText,sizeof(szText),"ERROR File Copy Error %s", vecPatchFileInfo[i].m_filePath);
						InterfaceManager::GetInstance()->SetAnnounceFirstLine(szText);
						return;
					}
				}
				::DeleteFileA(vecPatchFileInfo[i].m_filePath.Left( vecPatchFileInfo[i].m_filePath.GetLength() -4) );
			}

			if( !strOldUploaRoot.IsEmpty() )
				m_szUploadRoot = strOldUploaRoot;
		}
	}
	if(!WritePatch( m_szUploadRoot + PATCHINFO_FILE_NAME, false ) ) 
	{
		AfxMessageBox("Fail Write Patch Info!!!!!!!!!");
		return;
	}
	if(!PatchInfoUploadProcess())
	{
		AfxMessageBox("Fail Upload Patch Info!!!!!!!!!");
		return;
	}
	InterfaceManager::GetInstance()->SetAnnounceFirstLine("File Update Complete!");
}

#ifdef SHIPPING
CString CPatchManager::GetMyIPAddr()
{
	WSADATA wsaData;
	char szHostName[MAX_PATH];
	PHOSTENT hostInfo;
	CString ipAddr;
	if( WSAStartup( MAKEWORD(2,0), &wsaData) ==0 )
	{
		if( gethostname( szHostName, sizeof(szHostName)) == 0 )
		{
			hostInfo = gethostbyname( szHostName );
			ipAddr = inet_ntoa (*(struct in_addr *)hostInfo->h_addr_list);
		}
		WSACleanup();
	}
	return ipAddr;
}

bool CPatchManager::SendSyncFile()
{
	HINTERNET hInternet = NULL;
	HINTERNET hConnect = NULL;

	hInternet = InternetOpen("LostSaga", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (hInternet == NULL)
		return false;


	hConnect = InternetConnect(
		hInternet, "221.143.23.12", 7777,
		"lostsaga-up2", "0oW4OED)",
		INTERNET_SERVICE_FTP,
		INTERNET_FLAG_PASSIVE,
		NULL
		);
	if(!FtpSetCurrentDirectoryA(hConnect, "/ResultPatchSync/"))
		return false;
	if (hConnect)
	{
		bool b = FtpPutFile(
			hConnect,
			m_szServerType,
			m_szServerType +".txt",
			FTP_TRANSFER_TYPE_BINARY,
			NULL
			);
		assert( b );
	}

	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return true;
}



#endif

//이미 업데이트 파일 경로가 완성되어 있어야 한다.
bool CPatchManager::UploadIOP(IN CString& strPath, bool bPatchPac )
{
	if(!UploadFile(strPath, bPatchPac))
	{
		LOG.PrintTimeAndLog(0, "업로드 실패 : %s", (LPCTSTR)strPath);
		return false;
	}
	
	BackUpFileTXTNDDS(strPath);

	return true;
}

bool CPatchManager::WritePatchVersionUp( const CString &rszPath, bool bCheckExist )
{
	FILE    *file = NULL;
	file = ::fopen((LPCTSTR)rszPath,"wt");
	if(file == NULL) 
		return false;

	bool bDeleteFileList = false;
	if( rszPath.Find( DELETE_FILE_LIST_NAME ) != -1 )
		bDeleteFileList = true;

	int iSize = m_vVersionFileList.size();
	for(int i = 0;i < iSize;i++)
	{
		if( !bDeleteFileList )
		{
			if( bCheckExist )
			{
				if( !m_vFileList[i].m_bExist )
				{
					CString szHelp;
					szHelp.Format("No server file: %s", (LPCTSTR)m_vVersionFileList[i].m_file_path );
					InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
					LOG.PrintTimeAndLog(0, "No server file: %s", (LPCTSTR)m_vVersionFileList[i].m_file_path );
					continue;
				}
			}
		}

		if( m_vVersionFileList[i].m_file_path.IsEmpty() )
			continue;

		m_vVersionFileList[i].m_file_path.MakeLower();

		CString szList;
		szList = "[";
		if( bDeleteFileList )
		{
			szList += m_vVersionFileList[i].m_file_path;
		}
		else
		{
			szList += m_vVersionFileList[i].m_file_path + ":";
			szList += m_vVersionFileList[i].m_crc + ":";
			szList += m_vVersionFileList[i].m_file_size + ":";
			szList += m_vVersionFileList[i].m_unzip_file_size;
		}
		szList += "]";

		::fprintf(file, "%s\n",(LPCTSTR)szList);
	}
	::fclose( file );	
	return true;
}

void CPatchManager::RunMakePatchPac()
{
	m_iVersionFileState = VERSION_INFO_NONE; //VersionPatchInfo 상태 설정
	m_bVersionUp = TRUE;
	if( !m_bVersionUp )
	{
		LOG.PrintTimeAndLog( 0,"%s - Version Up 관련 설정을 확인해 주시길 바랍니다",__FUNCTION__);
		MessageBox(NULL,"Please. check vsersion up setting","CHECK",MB_OK);
		return;
	}
	InitFileList();
	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if( !CheckVersionPatchInfo() ) //현재 만들 버전파일의 버전을 설정
	{
		AfxMessageBox("Download Fail! Version Patch Info!!!!!!!");
		return;
	}

	m_iVersionFileState = VERSION_INFO_NONE; //version_patch.cfg 상태 설정
	if( !bDownPatchInfo( m_bVersionUp ) )
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}
	if( m_iVersionFileState != VERSION_INFO_OK )
	{
		LOG.PrintTimeAndLog( 0, "%s - 버전업 관련 파일을 확인해 주시길 바랍니다.", __FUNCTION__ );
		MessageBox( NULL, "Plese Check the Versionup File", "CHECK",MB_OK );
		return;
	}

	CString szPacPath = "config/";
	szPacPath += VERSIONPATCH_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR) szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR) szPacPath ), GetFileSize( (LPCTSTR) szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR) szPacPath )); // unzip size

	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile( szPacPath ))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	CString szParsePath = "config\\";
	szParsePath += VERSIONPATCH_FILE_NAME;
	if(!PatchParse((LPCTSTR)szParsePath, m_bVersionUp)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}

	//check patch pac file
	if( m_strCurrentVersion.IsEmpty() ) 
	{
		AfxMessageBox("CurrentVersion Info Empty!!!!!!!");
		return;	
	}

	//pac divide check to size //팩 나누자~~
	int iVersion = _ttoi(m_strCurrentVersion);
	++iVersion;
	m_strCurrentVersion.Format("%05d",iVersion);
	CreatePatchPac( m_strCurrentVersion );

	m_iPacDivideSize; //kb
	int iCurrPacSize = 0;

	for( int i = 0 ; i < m_vVersionFileList.size() ; ++i )
	{
		if( !DownloadPatchFile( /*CString("config/")*/CString(""), m_vVersionFileList[i].m_file_path ) )
		{
			MessageBox( NULL, "Pac file Download Fail", "ERROR", MB_OK );
			return;
		}

		CString szDownloadFullPath;// = "config/";
		szDownloadFullPath += m_vVersionFileList[i].m_file_path;
		char szComment[MAX_PATH]="";
		g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)szDownloadFullPath );
		char szPacPassword[MAX_PATH]="";
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );

		if( CPacUtil::ReturnValue::RV_OK !=  g_Pac.UnPacPieceFileWithPassword( (LPCTSTR)szDownloadFullPath, szPacPassword ) )
		{
			LOG.PrintTimeAndLog(0, "%s - 파일을 열수 없습니다. 확인해주세요. %s" ,__FUNCTION__, szDownloadFullPath);
			MessageBox(NULL, "Can Not Open the File \nPlease. Check the IOP File", "ERROR" ,MB_OK);
			continue;
		}



		DWORD dwCRC = 0;
		CString strName = szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4);
		CCrc32Static::FileCrc32Streams((LPCTSTR)strName, dwCRC);
		char* stop;
		DWORD dwServerPatchCRC = strtoul(m_vVersionFileList[i].m_crc, &stop,16); //진수 변환
		if( dwServerPatchCRC != dwCRC )
		{
			LOG.PrintTimeAndLog(0, "%s - 다운로드한 파일의 CRC값이 다릅니다. 파일을 확인해주세요. %s" ,__FUNCTION__, szDownloadFullPath);
			MessageBox(NULL, "Can Not Mached the File -\nPlease. Check the IOP File", "ERROR", MB_OK);
		}
		else
		{
			//Decrypt
			CString szTemp = szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4);
			szTemp.MakeLower();
			szTemp.MakeReverse();
			if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
			{
				if( !DecryptFile( szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4) ) )
					return;
			}
			LOG.PrintTimeAndLog( 0 , "%s add pac file  namw %s pw %s, comm %s", __FUNCTION__, szDownloadFullPath, szPacPassword, szComment );
			if( !g_PacChild( NEW_PATCH_PAC )->AddBigPac( (LPCTSTR)szDownloadFullPath, (LPCTSTR)(szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4)), time(NULL), szPacPassword, szComment ) )
			{
				LOG.PrintTimeAndLog(0, "%s - 팩 파일 추가에 실패하였습니다.. %s" ,__FUNCTION__, szDownloadFullPath);
				MessageBox(NULL, "Pac Fail","Err", MB_OK);
				return;
			}
			DeleteFile(szDownloadFullPath);
			DeleteFile(szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4));
		}

		iCurrPacSize += _ttoi(m_vVersionFileList[i].m_file_size);
		if( m_iPacDivideSize < (iCurrPacSize / 1024) ) //pac size over -> new pac
		{
			g_PacChild( NEW_PATCH_PAC )->CloseBigPac();
			++m_iPacSubNum;
			CreatePatchPac( m_strCurrentVersion, m_iPacSubNum );
			iCurrPacSize = 0;
		}
	}
	g_PacChild( NEW_PATCH_PAC )->CloseBigPac();

	if( !UploadPacthPactoAll() )
	{
		MessageBox( NULL, "Fail Upload Patch Pac File", "ERROR", MB_OK );
		return;
	}
}

//팩파일의 버전을 설정한다.
bool CPatchManager::CheckVersionPatchInfo()
{
	CString szHelp;
	szHelp.Format("Download : %s", (LPCTSTR)VERSIONPATCHINFO_FILE_NAME);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	CString szDownPath = "config/";
	szDownPath += VERSIONPATCHINFO_FILE_NAME;

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);

	int iCnt = 10;
	int iRemoteSize = 0;
	while(iCnt > 0)
	{
		if(!m_FTP.DownloadFile( szDownPath, m_szVersionRoot + VERSIONPATCHINFO_FILE_NAME, m_bPassive, &iRemoteSize ))
		{
			if( iRemoteSize == 0 )
			{
				if( IDYES == MessageBox( NULL, "Can Not Found File(/version/Version_Patch_Info.txt)\nDo you want New Version_Patch_Info File\n(DefaultVersion:v00001)", "WARRING", MB_YESNO ) )
				{
					LOG.PrintTimeAndLog(0, "버전 정보 파일 생성 설정: %s", (LPCTSTR)szDownPath);
					m_iVersionFileState = VERSION_INFO_CREATE;
					m_strCurrentVersion = "00000";
					return true;
				}
			}
			m_FTP.Abort();
			Sleep(1000);
			ReConnect();
			LOG.PrintTimeAndLog(0, "다운로드 RECONNECT : %s", (LPCTSTR)szDownPath);
		}
		else
		{
			break;
		}

		iCnt--;
		Sleep(1000);
	}

	if( iCnt == 0)
	{
		CString szHelp;
		szHelp.Format("Fail Downloading files : %s", (LPCTSTR)szDownPath);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}

	SetUseDisplaySec(false);

	//read version Patch info
	FILE *fp;
	char buffer[MAX_PATH];
	CString strInfo;
	CString szTemp;
	m_vecOldPatchInfo.clear();
	int iMaxList = 0;
	strcpy(buffer, szDownPath);
	fp = fopen(buffer,"rt");
	if(fp == NULL)
		return false;
	do
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		strInfo = buffer; //맨 위에 줄만 사용한다.
		if( strInfo.IsEmpty() )
			break;

		if( -1 != strInfo.Find("PatchVersion"))
		{
			for( int i = 0 ; i < strInfo.GetLength() ; ++i )
			{
				char ch = strInfo.GetAt(i);
				if( ch >= '0' && ch <= '9' )
				{
					m_strCurrentVersion += ch;
				}
			}
		}

		if( -1 != strInfo.Find("PatchInfo"))
		{
			m_vecOldPatchInfo.push_back(strInfo);
		}

	}while(!feof(fp));
	fclose(fp);

	SetFileAttributes(szDownPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szDownPath);

	return true;
}

bool CPatchManager::DownloadPatchFile( CString& LocalPath, CString& RemotePath )
{
	CString szHelp;
	szHelp.Format("Download : %s", (LPCTSTR)RemotePath);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
	CString szLocalPath = LocalPath;
	szLocalPath += RemotePath;

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);

	if( szLocalPath.Find('/') != -1 ) //다운로드 경로 폴더 생성
	{
		TCHAR path[_MAX_PATH];
		GetModuleFileName(NULL, path, sizeof path);
		char drive[MAX_PATH] = "";
		char dir[MAX_PATH] = "";
		::_splitpath(path ,drive,dir,NULL,NULL);
		CString ExtractPath = drive;
		ExtractPath += dir;
		CString MakeDir = szLocalPath.Left( szLocalPath.ReverseFind('/') );
		MakeDir.Replace("/","\\");
		SetCreateDirectory( ExtractPath, MakeDir); //하위 폴더 생성
	}
	int iCnt = 10;
	int iRemoteSize = 0;
	while(iCnt > 0)
	{
		if(!m_FTP.DownloadFile( szLocalPath, m_szPatchRoot+ RemotePath, m_bPassive, &iRemoteSize ))
		{
			if( iRemoteSize == 0 )
			{
				CString szText;
				szText.Format("Please Check the File : %s\nRemote FileSize is 0", (LPCTSTR)RemotePath);
				return false;
			}
			m_FTP.Abort();
			Sleep(1000);
			ReConnect();
			LOG.PrintTimeAndLog(0, "다운로드 RECONNECT : %s", (LPCTSTR)RemotePath);
		}
		else
		{
			break;
		}

		iCnt--;
		Sleep(1000);
	}

	if( iCnt == 0)
	{
		CString szHelp;
		szHelp.Format("Fail Downloading files : %s", (LPCTSTR)RemotePath);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}

	SetUseDisplaySec(false);
	return true;	
}

bool CPatchManager::CreatePatchPac( CString& szPacVersion, IN int iSubNum)
{
	char szTemp[MAX_PATH]="";
	m_srtNewPatchFile = "";
	StringCbPrintf( szTemp, sizeof(szTemp), "%d", NEW_PATCH_PAC);
	g_Pac.Add( szTemp );
	//int iVersion = _ttoi(szPacVersion);
	//++iVersion;
	//szPacVersion.Format("%05d",iVersion);
	//m_strCurrentVersion = szPacVersion;
	m_srtNewPatchFile = "config/v";
	m_srtNewPatchFile += szPacVersion;
	if( iSubNum != 0 )
	{
		CString szSubNum;
		szSubNum.Format("%02d",iSubNum);
		m_srtNewPatchFile += "_";
		m_srtNewPatchFile += szSubNum;
	}
	m_srtNewPatchFile += ".iop";

	LOG.PrintTimeAndLog(0, "New Add Start - %s.", m_srtNewPatchFile);
	DeleteFileRepeat( m_srtNewPatchFile );

	CFileException ex;
	CFile file;
	if(!file.Open(m_srtNewPatchFile,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) szError );
		return false;	
	}
	const int iDataSize = 22;
	BYTE szNewPacData[iDataSize]={80,75,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 빈 zip 파일 생성.
	file.Write((LPCSTR)szNewPacData,iDataSize);
	file.Close();    

	if( !g_PacChild(NEW_PATCH_PAC) )
	{
		LOG.PrintTimeAndLog(0, "New Pacth Pac Error");
		return false;
	}
	// pac 파일은 비번 설정 안함
	char szPacPassword[MAX_PASSWORD+1]="";
	enum { PASSWORD_TYPE = 1, };
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );

	/*	StringCbPrintf( szPacComment, sizeof( szPacComment ), "%d", PASSWORD_TYPE );*/


	// 	if(!PacFile(m_vFilePathList[i], szPacPassword, szPacComment ) )
	// 	{
	// 		LOG.PrintTimeAndLog(0, "팩 실패 : %s", (LPCTSTR)m_vFilePathList[i]);
	// 		return false;
	// 	}

	if(!g_PacChild(NEW_PATCH_PAC)->OpenBigPac(m_srtNewPatchFile, false, false, szPacPassword ) )
	{
		g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
		LOG.PrintTimeAndLog(0, "New Pactch Pac Open Error");
		return false;
	}
	return true;
}

bool CPatchManager::UploadPatchPac( IN CString& rszUploadRootFolder, IN CString& rszLocalPath )
{
	CString UploadFilePath;
	if( (rszLocalPath.Find("d:\\") == - 1) || (rszLocalPath.Find("c:\\") == - 1) )
	{
		char szDir[MAX_PATH] = "";
		::GetCurrentDirectory(MAX_PATH, szDir);
		UploadFilePath = szDir;
		UploadFilePath += '/';
	}
	//임시로 업데이트 디렉토리 변경
	UploadFilePath += rszLocalPath;

	m_srtNewPatchFile;
	CString strOldPatchRoot;
	strOldPatchRoot = m_szPatchRoot;
	m_szPatchRoot = rszUploadRootFolder;
	UploadFilePath.Replace("\\","/");
	UploadIOP(UploadFilePath, m_bVersionUp );
	m_szPatchRoot = strOldPatchRoot;
	return true;
}

bool CPatchManager::UploadPacthPactoAll()
{
	//버전 패치 경로
	CString UpdatePath = '/';
	UpdatePath += VERSION_FIOLDER;
	UpdatePath += '/';
	UpdatePath += m_strCurrentVersion;
	UpdatePath += '/';

	//pac파일 업로드
	m_srtNewPatchFile = "";
	m_srtNewPatchFile = "config/v";
	m_srtNewPatchFile += m_strCurrentVersion;
	m_srtNewPatchFile += ".iop";
	if( !UploadPatchPac( UpdatePath, m_srtNewPatchFile ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - 패치 팩 업로드에 실패했습니다. : %s ", __FUNCTION__ , m_srtNewPatchFile);
		return false;
	}

	if( m_iPacSubNum != 0 ) //sub file upload
	{
		for(int i = 0 ; i < m_iPacSubNum ; ++i )
		{
			CString szSubNum;
			szSubNum.Format("%02d",i + 1);

			m_srtNewPatchFile = "";
			m_srtNewPatchFile = "config/v";
			m_srtNewPatchFile += m_strCurrentVersion;
			m_srtNewPatchFile += "_";
			m_srtNewPatchFile += szSubNum;
			m_srtNewPatchFile += ".iop";
			if( !UploadPatchPac( UpdatePath, m_srtNewPatchFile ) )
			{
				LOG.PrintTimeAndLog( 0, "%s - 패치 팩 업로드에 실패했습니다. : %s ", __FUNCTION__ , m_srtNewPatchFile);
				return false;
			}
		}		
	}

	//버전 정보 파일 업로드
	CString strVersionPacInfoPath = "config/";
	strVersionPacInfoPath += VERSIONPATCH_FILE_NAME;
	strVersionPacInfoPath += ".iop";
	if( !UploadPatchPac( UpdatePath, strVersionPacInfoPath ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - 패치 팩 업로드에 실패했습니다. : %s ", __FUNCTION__ , strVersionPacInfoPath);
		return false;
	}

	//기존 버전 정보 파일 삭제
	CString strDeleteVerInfoFile = m_szPatchRoot;
	strDeleteVerInfoFile += VERSIONPATCH_FILE_NAME;
	strDeleteVerInfoFile += ".iop";
	if( !m_FTP.DeleteFileA(strDeleteVerInfoFile))
	{
		LOG.PrintTimeAndLog( 0 , "%s - FTP File Delete Fail -%s", __FUNCTION__, strDeleteVerInfoFile );
		MessageBox( NULL, "Delete Fail.\nOld Version_info File","ERROR", MB_OK );
	}

	//서버 버젼정보 갱신
	std::ofstream VersionInfoFile;	VersionInfoFile.open( VERSIONPATCHINFO_FILE_NAME, std::ios_base::out | std::ios_base::trunc );
	VersionInfoFile << "PatchVersion=" << m_strCurrentVersion << endl;
	for( int i = 0 ; i < m_vecOldPatchInfo.size() ; ++i )
	{
		VersionInfoFile << m_vecOldPatchInfo[i] << endl;
	}
	if(m_iPacSubNum != 0 )
	{
		VersionInfoFile << "PatchInfo=" << m_strCurrentVersion <<","<<m_iPacSubNum << endl;
	}

	VersionInfoFile.close();
	CString szHelp;
	szHelp.Format("Upload : %s", (LPCTSTR)VERSIONPATCHINFO_FILE_NAME);
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	SetUseDisplaySec(true);
	SetDisplayTitle(szHelp);
	CString strUpPath = '/';
	strUpPath += VERSION_FIOLDER;
	strUpPath += '/';
	int iCnt = 10;
	while( iCnt > 0)
	{
		if(!m_FTP.UploadFile(VERSIONPATCHINFO_FILE_NAME, strUpPath + VERSIONPATCHINFO_FILE_NAME, m_bPassive))
		{
			m_FTP.Abort();
			Sleep(1000);
			ReConnect();
			LOG.PrintTimeAndLog(0, "업로드 RECONNECT : %s", (LPCTSTR)VERSIONPATCHINFO_FILE_NAME );
		}
		else
		{
			break;
		}

		iCnt--;
		Sleep(1000);
	}

	if(iCnt == 0)
	{
		CString szHelp;
		szHelp.Format("Fail Uploading files : %s", (LPCTSTR)VERSIONPATCHINFO_FILE_NAME);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		return false;
	}
	SetUseDisplaySec(false);
	return true;
}

void CPatchManager::SyncPatchPac()
{
	m_iVersionFileState = VERSION_INFO_NONE; //VersionPatchInfo 상태 설정

	if( !m_bVersionUp )
	{
		LOG.PrintTimeAndLog( 0,"%s - Version Up 관련 설정을 확인해 주시길 바랍니다",__FUNCTION__);
		MessageBox(NULL,"Please. check vsersion up setting","CHECK",MB_OK);
		return;
	}
	InitFileList();
	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if( !CheckVersionPatchInfo() ) //현재 만들 버전파일의 버전을 설정
	{
		AfxMessageBox("Download Fail! Version Patch Info!!!!!!!");
		return;
	}

	//패치 파일 다운로드
	char szDir[MAX_PATH] = "";
	::GetCurrentDirectory(MAX_PATH,(LPSTR)szDir);
	char dir[MAX_PATH]	= "";
	::_splitpath((LPCTSTR)szDir,NULL,dir,NULL,NULL);

	CString strLocalDownFullPath = szDir;
	strLocalDownFullPath += "\\"; //config\\";

	CString strPatchName;
	strPatchName += "/v";
	strPatchName += m_strCurrentVersion;
	strPatchName += ".iop";

	CString szDownloadFolderPath = '\\';
	szDownloadFolderPath += VERSION_FIOLDER;
	szDownloadFolderPath += '\\';
	szDownloadFolderPath += m_strCurrentVersion;
	szDownloadFolderPath += '\\';

	CString szLocalName = "config" + szDownloadFolderPath;
	szLocalName += strPatchName;
	CString strOld = m_szPatchRoot;
	m_szPatchRoot = "";

	SetCreateDirectory(szDir, "/config" + szDownloadFolderPath);
	//szLocalName.Replace('\\','/');
	DownloadFile( szLocalName );
	szLocalName.Replace('/','\\');
	//patch pac 파일은 비밀번호가 설정되어 있지 않다.
	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof(szTemp), "%d", NEW_PATCH_PAC);
	g_Pac.Add( szTemp );

	// 	char szComment[MAX_PATH]="";
	// 	g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)vecPatchFileInfo[i].m_filePath );
	// 	char szPacPassword[MAX_PATH]="";
	// 	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	// 	if( pLocal )
	// 		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );

	// 	LOG.PrintTimeAndLog( 0 , "%s add pac file  namw %s pw %s, comm %s", __FUNCTION__, szDownloadFullPath, szPacPassword, szComment );
	// 	if( !g_PacChild( NEW_PATCH_PAC )->AddBigPac( (LPCTSTR)szDownloadFullPath, (LPCTSTR)(szDownloadFullPath.Left( szDownloadFullPath.GetLength() - 4)), time(NULL), szPacPassword, szComment ) )

	char szPacPassword[MAX_PASSWORD+1]="";
	char szPacComment[MAX_PATH]="";
	enum { PASSWORD_TYPE = 1, };
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );
	if( !g_PacChild(NEW_PATCH_PAC)->OpenBigPac(szLocalName, false, true, szPacPassword) ) 
	{
		LOG.PrintTimeAndLog( 0, "%s Patch Pac open fai %s", __FUNCTION__, szLocalName );
		return;
	}

	strPatchName = VERSIONPATCH_FILE_NAME;
	strPatchName += ".iop";
	szLocalName = "config" + szDownloadFolderPath;
	szLocalName += strPatchName;
	DownloadFile(szLocalName);
	m_szPatchRoot = strOld;

	//언팩
	CString szLocalPath = szDir;
	szLocalPath += "/config";
	szLocalPath += szDownloadFolderPath;
	szLocalPath +=	strPatchName;
	UnPacPieceFile( szLocalPath );

	if(!PatchParse((LPCTSTR)szLocalPath.Left( szLocalPath.GetLength() - 4), m_bVersionUp)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}
	m_vVersionFileList;


	if(!g_PacChild( NEW_PATCH_PAC )->InitSetIter())
	{
		g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
		LOG.PrintTimeAndLog(0, "%s PatchFile Fail InitSetIter", __FUNCTION__ );
		return;
	}

	int iMax = g_PacChild( NEW_PATCH_PAC )->GetSetSize();
	for( int i = 0 ; i < iMax ; ++i)
	{
		//get Pac File
		FileInfo kFileInfo;
		if(!g_PacChild(NEW_PATCH_PAC)->GetCurFileInfo(kFileInfo))
		{
			g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
			LOG.PrintTimeAndLog(0, "%s GetCurFileInfo Fail.", __FUNCTION__);
			return;
		}

		for( int j = 0 ; j < m_vVersionFileList.size() ; ++j )
		{
			CString strFilePath = kFileInfo.FileName.c_str();
			strFilePath.Replace('\\','/');
			if( strcmp( m_vVersionFileList[j].m_file_path , strFilePath) == 0 )
			{
				if( m_vVersionFileList[j].m_crc != g_PacChild(NEW_PATCH_PAC)->GetFileCRC(i) )
				{
					MessageBox(NULL, m_vVersionFileList[j].m_file_path, "CRC NOT MACHED", MB_OK);
					return;
				}
				else
					break; //정확히 일치
			}
		}
		CString szHelp;
		szHelp.Format("Extract Pac : %s", kFileInfo.FileName.c_str());
		CString szFileName = kFileInfo.FileName.c_str();
		szFileName.Replace("/","\\");
		CString MakeDir = szFileName.Left( szFileName.ReverseFind('\\') );
		MakeDir.Replace("/","\\");
		SetCreateDirectory( m_szUploadRoot, MakeDir); //하위 폴더 생성
		InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		if( !g_PacChild( NEW_PATCH_PAC )->Extract(i, m_szUploadRoot )) //해당 경로 폴더 생성해주자!!!
		{
			LOG.PrintTimeAndLog( 0 , "%s Extract Pac Error - %s" , __FUNCTION__, szFileName );
			MessageBox( NULL, "ERROR - Extract Pac" , "ERROR", MB_OK);
			return;
		}
		g_PacChild( NEW_PATCH_PAC )->NextSetIter();
	}
	//--- DecryptFile Check
	if(!g_PacChild( NEW_PATCH_PAC )->InitSetIter())
	{
		g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
		LOG.PrintTimeAndLog(0, "%s PatchFile Fail InitSetIter", __FUNCTION__ );
		return;
	}
	for( int i = 0 ; i < iMax ; ++i )
	{
		FileInfo kFileInfo;
		if(!g_PacChild(NEW_PATCH_PAC)->GetCurFileInfo(kFileInfo))
		{
			g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
			LOG.PrintTimeAndLog(0, "%s GetCurFileInfo Fail.", __FUNCTION__);
			return;
		}
		char szPacComment[MAX_PATH]="";
		g_PacChild( NEW_PATCH_PAC )->GetFileComment(szPacComment , sizeof(szPacComment), i );
		CString szFileName = kFileInfo.FileName.c_str();
		szFileName.MakeLower();
		szFileName.MakeReverse();
		if( szFileName.Find("ini." ) == 0 && atoi( szPacComment ) == 1 )
		{
			if( !DecryptFile( m_szUploadRoot +  kFileInfo.FileName.c_str()) )
			{
				LOG.PrintTimeAndLog(0 , "%s - Error DecryptFile %s", __FUNCTION__, kFileInfo.FileName.c_str());
				return;
			}
			CString szHelp;
			szHelp.Format("DecryptFile : %s", kFileInfo.FileName.c_str());
			InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
		}
		g_PacChild( NEW_PATCH_PAC )->NextSetIter();
	}
	//--- End DecryptFile


	g_PacChild( NEW_PATCH_PAC )->CloseBigPac();

	CString szHelp;
	szHelp.Format("Start Upload Process");
	InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);

	//m_iVersionFileState = VERSION_PAC_SYNC;
	m_bVersionUp = false;
	RunUpload();
	//m_iVersionFileState = VERSION_INFO_NONE;
}

//full.zip 작업할 부분
void CPatchManager::MakeFullzip()
{
	InitFileList();

	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if(!bDownPatchInfo(false))
	{
		AfxMessageBox("Fail Down Patch Info!!!!!!!");
		return;
	}

	CString szPacPath = "config/";  //파일을 다운받는 경로
	szPacPath += PATCHINFO_FILE_NAME;
	szPacPath += ".iop";
	LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u"
		, (LPCTSTR) szPacPath
		, g_Pac.GetPieceFileCRC( (LPCTSTR) szPacPath )
		, GetFileSize( (LPCTSTR) szPacPath )
		, g_Pac.GetPieceFileSize( (LPCTSTR) szPacPath )); // unzip size

	CString szUnzipName = szPacPath;
	szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
	SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile((LPCTSTR)szUnzipName);

	if(!UnPacPieceFile( szPacPath ))
	{
		AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
		return;
	}

	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;
	if(!PatchParse((LPCTSTR)szParsePath)) // 읽고 나서 정보 파일 삭제
	{
		AfxMessageBox("Fail Parse Patch Info!!!!!!!");
		return;
	}
	//m_vFileList로 전체 패치파일리스트 가져옴
	VPATCHINFOFILE vFileList;
	ifstream _File;
	char line[200];
	_File.open("FileList.txt");
	if(_File.is_open())
	{
		while(_File.getline(line, sizeof(line)))
		{
			for(size_t i = 1; i < m_vFileList.size(); ++i)//첫번째는 fullzip이기 때문
			{
				if(m_vFileList[i].m_file_path == line) 
				{
					CString str = m_vFileList[i].m_file_path.GetAt(0);
					if(str != " ")
						vFileList.push_back(m_vFileList[i]);
					break;
				}
			}
		}
	}
	_File.close();
	
	if(!vFileList.empty())
	{
		CString szLocalPath;
		for(size_t i = 0; i < vFileList.size(); ++i)
		{
			szLocalPath = "config/";

			szLocalPath += vFileList[i].m_file_path;
			if(!DownloadFile(szLocalPath, false))
			{
				LOG.PrintTimeAndLog(0, "다운로드 실패 : %s", (LPCTSTR)szLocalPath);
				vFileList.clear();
				return;
			}
		}
	}
	else
	{
		LOG.PrintTimeAndLog(0, "FileList is Empty");
		MessageBox(NULL,"파일 리스트가 없거나 비어있음","Error",MB_OK);
		return;
	}
	//fullzipfile list
	m_szPatchRoot = "/FullzipList/";
	CString szLocalPath;

	for(int i = 0; i < 2; ++i)
	{
		switch(i)
		{
		case 0:
			szLocalPath = "config/rs.iop";
			break;
		case 1:
			szLocalPath = "config/map.iop";
			break;
		default:
			MessageBox(NULL, "범위를 벗어남","Error", MB_OK);
			return;
		}
		if(!DownloadFile(szLocalPath, false))
		{
			LOG.PrintTimeAndLog(0, "다운로드 실패 : %s", (LPCTSTR)szLocalPath);
			vFileList.clear();
			return;
		}
		CFile pFile;
		if(!pFile.Open(szLocalPath,CFile::modeRead))
		{
			pFile.Close();
			MessageBox(NULL, "파일 열기 실패","Error", MB_OK);
			return;
		}
		PATCHINFOFILE pInfo;
		char szbuf[128]="";
		pInfo.m_file_path = szLocalPath.Right(szLocalPath.GetLength() - 7);
		vFileList.push_back(pInfo);
	}
	m_szPatchRoot = "/Client/";

	//full.zip으로 만들어줘야함
	CString szErrorMsg;
	CFileException ex;
	CFile file;
	szLocalPath = "config/full.zip";
	if(!file.Open(szLocalPath,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		szErrorMsg += szLocalPath;
		szErrorMsg += "\r\n";
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		szErrorMsg += szError;
		szErrorMsg += "\r\n";
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) szErrorMsg );
		MessageBox(NULL,"파일 생성 실패","Error",MB_OK);
		return;	
	}
	const int iDataSize = 22;
	BYTE szNewPacData[iDataSize]={80,75,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 빈 zip 파일 생성.
	file.Write((LPCSTR)szNewPacData,iDataSize);
	file.Close();    

	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof(szTemp), "%d", NEW_PATCH_PAC);
	g_Pac.Add( szTemp );



	if( !g_PacChild(NEW_PATCH_PAC) )
	{
		LOG.PrintTimeAndLog(0, "New Pacth Pac Error");
		return;
	}

	char szPacPassword[MAX_PASSWORD+1]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );

	if(!g_PacChild(NEW_PATCH_PAC)->OpenBigPac(szLocalPath, false, false, "" ) )
	{
		g_PacChild(NEW_PATCH_PAC)->CloseBigPac();
		LOG.PrintTimeAndLog(0, "New Pactch Pac Open Error");
		return;
	}

	for( size_t i = 0; i < vFileList.size(); ++i)
	{
		szLocalPath = "config/";
		szLocalPath+=vFileList[i].m_file_path;
		char szComment[MAX_PATH]="";
		g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)szLocalPath );
		char szPacPassword[MAX_PATH]="";

		pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal && vFileList[i].m_file_path != "rs.iop" && vFileList[i].m_file_path != "map.iop" )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi(szComment) );
		else
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );
		if( CPacUtil::ReturnValue::RV_OK !=  g_Pac.UnPacPieceFileWithPassword( (LPCTSTR)szLocalPath, szPacPassword ) )
		{
			LOG.PrintTimeAndLog(0, "%s - 파일을 열수 없습니다. 확인해주세요. %s" ,__FUNCTION__, szLocalPath);
			MessageBox(NULL, "Can Not Open the File \nPlease. Check the IOP File", "ERROR" ,MB_OK);
			continue;
		}		

		DWORD dwCRC = 0;
		CString strName = szLocalPath;
		if(i < vFileList.size())
			strName = szLocalPath.Left( szLocalPath.GetLength() - 4);

		CCrc32Static::FileCrc32Streams((LPCTSTR)strName, dwCRC);
		char* stop;
		DWORD dwServerPatchCRC = strtoul(vFileList[i].m_crc, &stop,16); //진수 변환
		if( dwServerPatchCRC != dwCRC && vFileList[i].m_file_path!="rs.iop.iop" && vFileList[i].m_file_path!="map.iop.iop" )
		{
			LOG.PrintTimeAndLog(0, "%s - 다운로드한 파일의 CRC값이 다릅니다. 파일을 확인해주세요. %s" ,__FUNCTION__, szLocalPath);
			MessageBox(NULL, "Can Not Mached the File -\nPlease. Check the IOP File", "ERROR", MB_OK);
		}
		else
		{
			//Decrypt
			CString szFileName = szLocalPath.Right(szLocalPath.GetLength() - 7); // config/만큼 삭제
			CString szTemp = szLocalPath;
			//if(i < vFileList.size())
			//	szTemp = szLocalPath.Left( szLocalPath.GetLength() - 4);
			szTemp.MakeLower();
			szTemp.MakeReverse();
			if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
			{
				if( !DecryptFile( szLocalPath.Left( szLocalPath.GetLength() - 4) ) )
					return;
			}
			LOG.PrintTimeAndLog( 0 , "%s add pac file  namw %s pw %s, comm %s", __FUNCTION__, szLocalPath, szPacPassword, szComment );
			CString szHelp;
			szHelp.Format("Add To Full.zip : %s", (LPCTSTR)szLocalPath);
			InterfaceManager::GetInstance()->SetAnnounceFirstLine(szHelp);
			SetUseDisplaySec(true);
			SetDisplayTitle(szHelp);

			if(i<vFileList.size()-2)
			{			
				if( !g_PacChild( NEW_PATCH_PAC )->AddBigPac( (LPCTSTR)szLocalPath, (LPCTSTR)(szFileName.Left( szFileName.GetLength() - 4)), time(NULL), szPacPassword, szComment ) )
				{
					LOG.PrintTimeAndLog(0, "%s - 팩 파일 추가에 실패하였습니다.. %s" ,__FUNCTION__, szLocalPath);
					MessageBox(NULL, "Pac Fail","Err", MB_OK);
					return;
				}
			}
			else
			{
				CFile pFile;
				if(!pFile.Open(szLocalPath,CFile::modeRead))
				{
					LOG.PrintTimeAndLog(0, "%s - Open File Fail.. %s" ,__FUNCTION__, szLocalPath);
					MessageBox(NULL, "Pac Fail","Err", MB_OK);
					return;
				}
				DWORD uSize = pFile.GetLength();
				char* pData = new char[uSize];
				pFile.Read(pData,uSize);

				if( !g_PacChild( NEW_PATCH_PAC )->AddBigPacData( szLocalPath.Right(szLocalPath.GetLength() - 7), (BYTE*)pData, uSize, ""))
				{
					LOG.PrintTimeAndLog(0, "%s - 팩 파일 추가에 실패하였습니다.. %s" ,__FUNCTION__, szLocalPath);
					MessageBox(NULL, "Pac Fail","Err", MB_OK);
					if(pData!= NULL)
					{
						delete [] pData;
						pData = NULL;
					}
					return;
				}
				if(pData!= NULL)
				{
					delete [] pData;
					pData = NULL;
				}
				pFile.Close();
			}
			DeleteFile(szLocalPath);
			DeleteFile(szLocalPath.Left( szLocalPath.GetLength() - 4));
		}
	}
	g_PacChild(NEW_PATCH_PAC)->CloseBigPac();

	if(!::MoveFile("config/full.zip",m_szUploadRoot+"full.zip"))
		MessageBox(NULL,"이동하려는 대상이 이미 존재합니다","Error",MB_OK);
	else		
		RunUpload();

	InterfaceManager::GetInstance()->SetMakeFullzipEnable(TRUE);
	
}


bool CPatchManager::DecryptFile( const CString &rszFileName )
{
	// read
	CFile file;
	if( !file.Open((LPCTSTR)rszFileName, CFile::modeRead | CFile::shareDenyNone) )
	{
		return false;
	}

	int iSize = (int) file.GetLength();

	char *pData = new char[iSize];
	if( file.Read( pData, iSize ) != iSize )
	{
		delete [] pData;
		file.Close();
		return false;
	}

	file.Close();

	// enrypt
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		delete [] pData;
		LOG.PrintTimeAndLog( 0, "%s pLocal == NULL.", __FUNCTION__ );
		return false;
	}

	pLocal->EncryptDecryptData( pData, iSize, pData, iSize, false );

	// save
	if( !DeleteFileRepeat( rszFileName ) )
	{
		delete [] pData;
		return false;	
	}

	if(!file.Open((LPCTSTR)rszFileName,CFile::modeCreate | CFile::modeWrite))
	{
		delete [] pData;
		return false;	
	}
	file.Write( pData, iSize );
	file.Close();  
	delete [] pData;
	return true;
}

void CPatchManager::MakeDeleteFileList()
{
	FixServerCfg(true);
}

bool CPatchManager::FixServerCfgforDelete(CString &szDeletePath)
{
	CString szParsePath = "config\\";
	szParsePath += PATCHINFO_FILE_NAME;

	FILE * fp = NULL;
	CEditDlg	Editdlg;
	if(!fp)
	{
		if(!ConnectProcess())  //접속
		{
			AfxMessageBox("Fail Connect!!!!!!!");
			return false;
		}

		if(!bDownPatchInfo(false))
		{
			AfxMessageBox("Fail Down Patch Info!!!!!!!");
			return false;
		}

		CString szPacPath = "config/";
		szPacPath += PATCHINFO_FILE_NAME;
		szPacPath += ".iop";
		LOG.PrintTimeAndLog(0, "Completed Download : %s : %x : %u : %u", (LPCTSTR)szPacPath, g_Pac.GetPieceFileCRC( (LPCTSTR)szPacPath )
			, GetFileSize( (LPCTSTR)szPacPath ), g_Pac.GetPieceFileSize( (LPCTSTR)szPacPath )); // unzip size

		CString szUnzipName = szPacPath;
		szUnzipName = szUnzipName.Left( szUnzipName.GetLength() - 4); // .iop 을 삭제함.
		SetFileAttributes((LPCTSTR)szUnzipName,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile((LPCTSTR)szUnzipName);

		if(!UnPacPieceFile(szPacPath))
		{
			AfxMessageBox("Fail UnPack Patch Info!!!!!!!");
			return false;
		}

		CString str;

		m_vFileList.clear();
		if(!PatchParse((LPCTSTR)szDeletePath)) // 읽고 나서 정보 파일 삭제
		{
			AfxMessageBox("Fail Parse Patch Info!!!!!!!");
			return false;
		}

		if( m_vFileList.empty() )
		{
			AfxMessageBox("Empty delete file list!!!!!!!");
			return false;
		}

		Editdlg.SetLoadFilePath(szUnzipName);
		MakeNewServerPatch(szUnzipName);
		
		DeleteServerFile(szDeletePath, m_vFileList);
	}
	else
	{
		fclose(fp);
		LOG.PrintTimeAndLog(0, "Load Auto File");
	}

	return true;
}

void CPatchManager::DeleteServerFile(const CString szPath, const VPATCHINFOFILE &vFileList)
{
	if(!ConnectProcess()) 
	{
		AfxMessageBox("Fail Connect!!!!!!!");
		return;
	}

	if( vFileList.empty() )
	{
		AfxMessageBox("Empty delete file list!!!!!!!");
		return;
	}

	CFileFind CFileFinder;
	CString szBackupData="", strDate;
	COleDateTime NowDate;
	NowDate = COleDateTime::GetCurrentTime();
	strDate.Format( "%04d%02d%02d", NowDate.GetYear(), NowDate.GetMonth(), NowDate.GetDay());
	m_szBackUpRoot += strDate+"\\";
	if(!CFileFinder.FindFile(m_szBackUpRoot)) //디렉토리가 음슴
		CreateDirectory(m_szBackUpRoot, NULL);

	// 서버파일삭제
	const int iMax = vFileList.size();
	InterfaceManager::GetInstance()->SetPatchGauge(iMax,0);

	for (int i = 0; i < iMax ; i++)
	{
		if(vFileList[i].m_file_path.IsEmpty())
			continue;
		CString Cmp=" ";
		if(vFileList[i].m_file_path.GetAt(0) == Cmp)
		{
			InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
			continue;
		}

		CString strReplace = vFileList[i].m_file_path;
		strReplace.Replace("/","\\");
		szBackupData = m_szBackUpRoot + strReplace;
		char szDrive[MAX_PATH]= "";
		char szDir[MAX_PATH*2]= "";
		char szDriveDir[MAX_PATH*2]= "";
		::_splitpath((LPCTSTR)szBackupData,szDrive,szDir,NULL,NULL);
		StringCbCopy( szDriveDir , sizeof( szDriveDir ), szDrive );
		StringCbCat( szDriveDir , sizeof( szDriveDir ), szDir );
		SetCreateDirectoryByFullPath( szDriveDir );
		
		// delete Process
		CString szServerPath = m_szPatchRoot + vFileList[i].m_file_path;
		szServerPath.Replace("\\", "/");
		int iRemotesize = 0;

		//없는파일 폴더 경로 만들어줌.
		//다운로드 경로 만들어줌 다운로드 받을 경로 서버 주소

		LOG.PrintTimeAndLog(0, "Backup Start... : %s", szServerPath);
		InterfaceManager::GetInstance()->SetAnnounceFirstLine("Backup Start... : "+szServerPath);
		if(!m_FTP.DownloadFile(szBackupData, szServerPath, false, &iRemotesize))
		{
			LOG.PrintTimeAndLog(0, "백업 실패. 해당 파일을 찾을수 없습니다 : %s", szServerPath);
			InterfaceManager::GetInstance()->SetAnnounceFirstLine("Backup Fail !! can't find  : "+szServerPath);
			::DeleteFile((LPCTSTR)szBackupData);
			InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
			continue;
		}



		CString szWarningMsg = "Please Backup this File.\nif you delete can't restore file\n";
		szWarningMsg += "("+szServerPath+")\ndo you delete file?";
		
		if(MessageBox(NULL,szWarningMsg,"File Delete", MB_ICONWARNING | MB_YESNO) == IDYES) //6 ==yes 
		{
			if(m_FTP.DeleteFile(szServerPath))	//파일이 지워져있어도 성공메시지
			{
				LOG.PrintTimeAndLog(0, "Completed Server File Delete : %s", szServerPath);
				InterfaceManager::GetInstance()->SetAnnounceFirstLine("Completed Server File Delete :"+szServerPath);
			}
			else
			{
				LOG.PrintTimeAndLog(0, "Fail Server File Delete : %s GetLastError: %d", szServerPath, GetLastError());
				InterfaceManager::GetInstance()->SetAnnounceFirstLine("Fail Server File Delete :"+szServerPath);
			}
		}
		
		InterfaceManager::GetInstance()->SetPatchGauge(iMax,i+1);
		
	}
	m_szBackUpRoot.Replace(strDate+"\\", "");
}


void CPatchManager::SetCreateDirectoryByFullPath( const char *szDir )
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

void CPatchManager::FullZipEnd()
{
	InterfaceManager::GetInstance()->SetCheckFileSyncEnable(FALSE);
	InterfaceManager::GetInstance()->SetUpdateBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetUploadBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetCreateInfoBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetDeleteListBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetChangeFullZipBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetDelNoServerBtnEnable(FALSE);
	InterfaceManager::GetInstance()->SetMakePatchEnable(FALSE);
	InterfaceManager::GetInstance()->SetPatchInfoFixEnable(FALSE);
	InterfaceManager::GetInstance()->SetShowLogBtnEnable(TRUE);
	InterfaceManager::GetInstance()->SetMakeDelFileEnalbe(FALSE);
	InterfaceManager::GetInstance()->SetMakeFullzipEnable(TRUE);
}

void CPatchManager::MakeNewServerPatch(CString &szPath)
{
	CStdioFile	Txt_File;
	CString strTxt;
	m_vServerPatchStr.clear();
	//속도개선은 m_vFileList를 List로 변경하자
	if(Txt_File.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		CString		strToken;
		while(Txt_File.ReadString(strToken))
		{
			CString test;	

			bool bDelete = false;
			for(size_t i = 0; i < m_vFileList.size(); ++i)
			{
				test = m_vFileList[i].m_file_path;
				test = "[" + test;
				int iIndex = strToken.Find(test);
				if(iIndex == -1)
					continue;
				
				if(strToken.GetAt(iIndex) == CString("/"))// "["이 나와야 값이같은것
					continue;
				else
				{
					if(!strncmp(strToken, test, test.GetLength()))
					{
						bDelete = true;
						break;
					}
				}
			}
			if(!bDelete)
			{
				m_vServerPatchStr.push_back(strToken+"\n");
			}
		}
	}
	Txt_File.Close();
	::DeleteFile((LPCTSTR)szPath);
}
