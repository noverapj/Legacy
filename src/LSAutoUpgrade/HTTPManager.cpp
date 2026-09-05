// HTTPManager.cpp: implementation of the HTTPManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "AutoUpgradeDlg.h"
#include "HTTPManager.h"
#include <mmsystem.h>
#include <intshcut.h>
#include "ioINILoaderAU.h"
#include "CopyFolder.h"
#include "HelpFunc.h"
#include "Crc32Static.h"
#include "Version.h"
#include "StringManager/safesprintf.h"
#include "StringManager/ioStringManager.h"
#include "../ioPac/PacErrorReport.h"
#include <strsafe.h>
#include "Local/ioLocalParent.h"
#include "Local/ioLocalManager.h"
#include "Local/ioLocalUS.h"
#include "Local/ioLocalBrazil.h"
#include "Util/ioHttpManager.h"

#ifdef XTRAP
#include "Xtrap/ioXtrap.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <sstream>
#include <fstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HTTPManager::HTTPManager() : m_bActive(false) , m_file_count(0), m_iMaxPac(0)
{
	m_vFileList.reserve(1000);
	m_vPatchPacList.reserve(1000);

	for (int i = 0; i < MAX_FILE_PAC ; i++)
	{
		m_vDeleteFileList[i].reserve(1000);	
		m_vListDeleteFile[i].reserve(1000);
	}

	memset( m_szRootDir, 0 , sizeof(m_szRootDir) );
	GetCurrentDirectory(MAX_PATH,m_szRootDir);
	
	memset( m_szPacFullPath, 0, sizeof(m_szPacFullPath) );
	StringCbPrintf(m_szPacFullPath[RS_PAC], sizeof(m_szPacFullPath[RS_PAC]), "%s\\rs.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_PAC], sizeof(m_szPacFullPath[MAP_PAC]), "%s\\map.iop", m_szRootDir);
#ifdef SRC_EU
	StringCbPrintf(m_szPacFullPath[MAP_EN], sizeof(m_szPacFullPath[MAP_EN]), "%s\\map_en.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_DE], sizeof(m_szPacFullPath[MAP_DE]), "%s\\map_de.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_FR], sizeof(m_szPacFullPath[MAP_FR]), "%s\\map_fr.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_IT], sizeof(m_szPacFullPath[MAP_IT]), "%s\\map_it.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_PL], sizeof(m_szPacFullPath[MAP_PL]), "%s\\map_pl.iop", m_szRootDir);
	StringCbPrintf(m_szPacFullPath[MAP_TR], sizeof(m_szPacFullPath[MAP_TR]), "%s\\map_tr.iop", m_szRootDir);
#endif
	
	
#ifdef SRC_EU
	m_iMaxPac = MAX_FILE_PAC;
#else
	m_iMaxPac = 2;
#endif

#ifdef SHIPPING
	m_bOverlapCheck = true;
#endif
	m_bRunThread = false;
	m_bAdminPatch = false;
	m_eRunEndType = ET_EXIT;
	m_iDisplayCnt = 1;
	m_dwAddPacSize = 0;
	m_bWaitBeforeNewpac = true;
	m_hWaitEvent        = NULL;
	m_iOptimizeBar		= 0;
	m_bShowGauge		= false;
	m_iRetry			= 0;
	LoadINI();
}

HTTPManager::~HTTPManager()
{
	m_vFileList.clear();
	m_vPatchPacList.clear();
	for (int i = 0; i < MAX_FILE_PAC ; i++)
		m_vDeleteFileList[i].clear();
	g_Pac.ReleaseInstance();
	if( m_hWaitEvent )
	{
		CloseHandle( m_hWaitEvent );
		m_hWaitEvent = NULL;
	}
}

void HTTPManager::Close()
{
	m_bActive = false;	
	ResumeRun();
}

void HTTPManager::Run()
{
__try
{
	if(!InitPac())
	{
		return;
	}

#ifdef SHIPPING
	_RunShipping();
#else // _ADMIN_
	_RunAdmin();
#endif
}
__except(1)
{
	ShowErrorDlg(STR(1),209);
	SendLOG( "Occur except", 209);
}
}

void HTTPManager::_RunShipping()
{
	m_pAutoupDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	_RunInit();
	CopyVersionupFiles();

	if(!_Run())
	{
		g_PacChild(RS_PAC)->CloseBigPac();
		g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SRC_EU
		g_PacChild(MAP_EN)->CloseBigPac();
		g_PacChild(MAP_DE)->CloseBigPac();
		g_PacChild(MAP_FR)->CloseBigPac();
		g_PacChild(MAP_IT)->CloseBigPac();
		g_PacChild(MAP_PL)->CloseBigPac();
		g_PacChild(MAP_TR)->CloseBigPac();
#endif
		LOG.PrintTimeAndLog(0, "Fail - Run - _RunShipping");
	}
	else
	{
		LOG.PrintTimeAndLog(0, "Complete - Run - _RunShipping");
		StartGame();
	}


	_RunEnd();

}

void HTTPManager::_RunAdmin()
{
#ifdef _ADMIN_
	_RunInit();
	CopyVersionupFiles();
	// option
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL)
	{
		_RunEnd();
		LOG.PrintTimeAndLog(0, "Error - Run - pDlg == NULL");
		return;/////
	}

	if(pDlg->IsExtractPac())
	{
		for (int i = 0; i < m_iMaxPac ; i++)
		{
			if( !UnPac(m_szPacFullPath[i], i+1, m_iMaxPac) )
			{
				char szTempMsg[MAX_PATH*4]="";
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), i+1, (LPCTSTR) m_szErrorMsg );
				ShowErrorDlg(szTempMsg, 0);
				_RunEnd();
				return; /////
			}	
		}

		if(m_bActive)
			SetText( STR(2));
		_RunEnd();
		LOG.PrintTimeAndLog(0, "Complete - Run - ExtractPac");
		return;/////
	}

	if(!_RunOptionCopyFolder(pDlg))
	{
		CString szMsg = STR(3);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg, 0);
		LOG.PrintTimeAndLog(0, "Error - Run - CopyFolder");
		_RunEnd();
		return;/////
	}
	//

	// Copy 이후에 Admin 파일 다운로드 위해서
	if(pDlg->IsCopyFolder())
		LoadINI();
	else
	{
		if(!_Run())
		{
			g_PacChild(RS_PAC)->CloseBigPac();
			g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SRC_EU
			g_PacChild(MAP_EN)->CloseBigPac();
			g_PacChild(MAP_DE)->CloseBigPac();
			g_PacChild(MAP_FR)->CloseBigPac();
			g_PacChild(MAP_IT)->CloseBigPac();
			g_PacChild(MAP_PL)->CloseBigPac();
			g_PacChild(MAP_TR)->CloseBigPac();
#endif
		}
	}

	m_bAdminPatch = true;
	m_szPatchURL = m_szAdminPatchURL;

	if(!_Run()) 
	{
		g_PacChild(RS_PAC)->CloseBigPac();
		g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SHIP_EU
		g_PacChild(MAP_EN)->CloseBigPac();
		g_PacChild(MAP_DE)->CloseBigPac();
		g_PacChild(MAP_FR)->CloseBigPac();
		g_PacChild(MAP_IT)->CloseBigPac();
		g_PacChild(MAP_PL)->CloseBigPac();
		g_PacChild(MAP_TR)->CloseBigPac();
#endif		
		_RunEnd();
		return;///
	}

	if(pDlg->IsVSDebug())
	{	
		ioINILoaderAU kLoader("config\\debug_connect.ini");
		kLoader.SetTitle("debug_connect");
		kLoader.SaveString("login_key", AfxGetApp()->m_lpCmdLine);
		LOG.PrintTimeAndLog(0, "Complete - Run - VSDebug");
	}
	else if(pDlg->IsCopyFolder())
	{
		// don't run startgame().
		LOG.PrintTimeAndLog(0, "Complete - Run - CopyFolder");
	}
	else
		StartGame();

	_RunEnd();

	LOG.PrintTimeAndLog(0, "Complete - Run - _RunAdmin");

#endif // _ADMIN_
}


bool HTTPManager::ParsePatchFile( bool bVersionInfo )
{
	if(bVersionInfo)
		m_vPatchPacList.clear();
	else
		m_vFileList.clear();

	FILE *fp=NULL;
	char buffer[MAX_PATH]="";
	char szPath[MAX_PATH*2]="";
	if( bVersionInfo )
	{
		StringCbPrintf(szPath, sizeof(szPath), "%s\\losadown\\version_patch.cfg", m_szRootDir);
	}
	else
	{
		StringCbPrintf(szPath, sizeof(szPath), "%s\\losadown\\server_patch.cfg", m_szRootDir);
	}
	
	fp = fopen(szPath,"rt");
	if(fp == NULL)
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1) ,szPath, GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg );
		return false;
	}
	do
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		int bufferSize = strlen(buffer);
		if( bufferSize != 0)
		{
			if(buffer[bufferSize-1] == '\n')
				buffer[bufferSize-1] = 0;
		}
		ParseOneLine(buffer, bVersionInfo);
	
	}while(!feof(fp));
	fclose(fp);

	if(!Help::DeleteFileRepeat( szPath ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1) ,szPath, GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg );
		return false;
	}

	// 미리 업데이트될 파일을 읽음(이후의 GetUpdateFile함수에서는 m_vecOverlap에 요소가 추가되지 않게 수정)
	while(true)
	{
		const PATCHINFOFILE *Next = GetUpdateFile(false, true);
		if(Next == NULL) 
		{
			m_file_count = 0;
			break;	
		}
	}

	if(!bVersionInfo)
		LOG.PrintTimeAndLog(0, "Complete parse patch info.");
	else
		LOG.PrintTimeAndLog(0, "Complete parse version patch info.");

	return true;
}

void HTTPManager::ParseOneLine( char *pSrc, bool bVersionInfo, bool bDelete )
{
	if(pSrc == NULL) return;
	
	char szFile[MAX_FILE_INFO][MAX_PATH];
	::memset(szFile,0,sizeof(szFile));
	int     len = ::strlen(pSrc);
	int     info_cnt = 0;
	int     info_size= 0;
	
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

	AddOneFileList(szFile, bVersionInfo, bDelete);
}


void HTTPManager::AddOneFileList( const char szFile[MAX_FILE_INFO][MAX_PATH],bool bVersionInfo, bool bDelete )
{
	char fName[MAX_PATH] = "";
	::_splitpath(szFile[0],NULL,NULL,fName,NULL);
	if(::strcmp(fName,"") == 0) 
		return;

	PATCHINFOFILE hf;	
	char *pTemp = NULL;
	hf.m_szRemoteFilePath	= szFile[0];
	hf.m_crc     	        = strtoul(szFile[1], &pTemp, 16);
	hf.m_file_size	        = atoi(szFile[2]);
	hf.m_unzip_file_size	= atoi(szFile[3]);
	hf.m_szLocalFilePath    = szFile[0];
	if(bDelete)
	{
		CString szInPath = hf.m_szLocalFilePath;
		if( strncmp( hf.m_szLocalFilePath, "_map", 4 ) == 0 )
		{			
			hf.m_szLocalFilePath = szInPath.Right( szInPath.GetLength() - 5); // _map/ 삭제
			hf.m_szLocalFilePath = hf.m_szLocalFilePath.Left( hf.m_szLocalFilePath.GetLength() - 4);
			//hf.m_szLocalFilePath.Replace("/","\\");
			m_vListDeleteFile[MAP_PAC].push_back(hf);
			hf.m_szLocalFilePath = szInPath;			
		}
		else if(  strcmp( hf.m_szLocalFilePath, "" ) != 0 )
		{
			hf.m_szLocalFilePath = hf.m_szLocalFilePath.Left( hf.m_szLocalFilePath.GetLength() - 4);
			//hf.m_szLocalFilePath.Replace("/","\\");
			m_vListDeleteFile[RS_PAC].push_back(hf);
			hf.m_szLocalFilePath = szInPath;
		}
	}

	strlwr(fName);
	if(::strcmp(fName, "full") != 0)
	{
		hf.m_szLocalFilePath = hf.m_szLocalFilePath.Left( hf.m_szLocalFilePath.GetLength() - 4); // .iop 삭제

		if( IsPutPacFileByPath( hf.m_szRemoteFilePath ) )
		{
			switch(IsMapPacFileByPath( hf.m_szRemoteFilePath ))
			{
			case FT_PAC_MAP:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 5); // _map/ 삭제
				hf.m_eFileType = FT_PAC_MAP;
				break;
			case FT_PAC_MAP_EN:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_en/ 삭제
				hf.m_eFileType = FT_PAC_MAP_EN;
				break;
			case FT_PAC_MAP_DE:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_de/ 삭제
				hf.m_eFileType = FT_PAC_MAP_DE;
				break;
			case FT_PAC_MAP_FR:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_fr/ 삭제
				hf.m_eFileType = FT_PAC_MAP_FR;
				break;
			case FT_PAC_MAP_IT:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_it/ 삭제
				hf.m_eFileType = FT_PAC_MAP_IT;
				break;
			case FT_PAC_MAP_PL:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_pl/ 삭제
				hf.m_eFileType = FT_PAC_MAP_PL;
				break;
			case FT_PAC_MAP_TR:
				hf.m_szLocalFilePath = hf.m_szLocalFilePath.Right( hf.m_szLocalFilePath.GetLength() - 8); // _map_tr/ 삭제
				hf.m_eFileType = FT_PAC_MAP_TR;
				break;
			default:
				hf.m_eFileType = FT_PAC_RS;
				break;
			}
		}
		else
		{
			if( ::strcmp(fName, "autoupgrade.exe") != 0)
				hf.m_eFileType = FT_PIECE;
			else
				hf.m_eFileType = FT_AUTOUPGRADE;
		}
	}
	else
	{
		hf.m_eFileType = FT_FULLZIP;
	}

#ifdef _ADMIN_
	if(m_bAdminPatch == false)
	{
		// 관리자용 파일과 겹치면 다른 폴더로 이동시킴
		if(::strcmp(fName, "io3dengine.dll") == 0
			||::strcmp(fName, "io3dengine.pdb") == 0
			||::strcmp(fName, "lostsaga.exe") == 0
			||::strcmp(fName, "lostsaga.pdb") == 0)
		{
			hf.m_szLocalFilePath.Format("%s%s","_Shipping/",szFile[0]);
			hf.m_szLocalFilePath = hf.m_szLocalFilePath.Left( hf.m_szLocalFilePath.GetLength() - 4);
		}
	}
#endif

	if( bVersionInfo )
		m_vPatchPacList.push_back(hf);
	else
		m_vFileList.push_back(hf);
}


bool HTTPManager::ResponseServerCheck()
{
//	SetText(STR(1));
	SetText("응답 대기 중");
	CTime cBaseTime = CTime::GetCurrentTime();
	cBaseTime += CTimeSpan(0,0,0,2); //2초
	int iCount = 15; //총 30초

	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	while(iCount)
	{
		if(pLocal->IsFine())
			return true;

		Sleep(1);
		bool bWait = true;
		while( bWait )
		{
			if( CTime::GetCurrentTime() >= cBaseTime )
			{
				bWait = false;
				LOG.PrintTimeAndLog(0, "waiting Server Respose - %d", iCount);
			}
		}
		cBaseTime = CTime::GetCurrentTime();
		cBaseTime += CTimeSpan(0,0,0,2); //2초

		--iCount;
	}

	return false;
}


HTTPManager::ReturnState HTTPManager::UpdateFileLoop()
{
	SetText(STR(1));
	m_file_count   = 0;
	m_dwAddPacSize = 0;
	m_bWaitBeforeNewpac = true;

#if _DEBUG
	enum { LIMIT_PATCH_SIZE = 5};
#else
	enum { LIMIT_PATCH_SIZE = 500};
#endif

	/***********************************************************************
	  국내 적용 내용 패치의 갯수가 LIMIT_PATCH_SIZE 가 넘어갈 경우, 패치리스트에 Full.zip을 넣는다.
	  Full.zip 적용 -> 추가 패치리스트 적용으로 수정
	  (cf. Full.zip은 최신화 유지시켜야함 안 그럴경우 Full.zip내의 오래된 파일로 패치가 적용될 수 있음)
	************************************************************************/
	if( g_LocalMgr.GetLocalType()  == ioLocalManager::LCT_KOREA )
	{
		//Patch list 1차 검사

		int nCount = 0;

		while(true)
		{
			if(!m_bActive) 
				return RETURN_STOP;
			
			const PATCHINFOFILE *Next = GetUpdateFile();
			if(Next == NULL) 
				break;

			nCount++;

			if(nCount > LIMIT_PATCH_SIZE)
			{
				//full.zip 삭제해줌
				CString szFullPath = m_szRootDir;
				szFullPath += "\\full.zip";
				if(!Help::DeleteFileRepeat( (LPCTSTR) szFullPath) )
				{
					char szTempMsg[MAX_PATH*4]="";
					StringCbPrintf( szTempMsg, sizeof( szTempMsg ), STR(3), 1, (LPCTSTR) szFullPath, GetLastError() );
					m_szErrorMsg = szTempMsg;

					ShowErrorDlg( (LPCTSTR) m_szErrorMsg ,222);
					SendLOG( m_szErrorMsg, 222);
					LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
					return RETURN_ERROR;
				}
				break;
			}
		}
	}

	m_file_count   = 0;
	m_dwAddPacSize = 0;
	m_bWaitBeforeNewpac = true;

	while(true)
	{
		Sleep(1);
		if(!m_bActive) 
			return RETURN_STOP;

		const PATCHINFOFILE *Next = GetUpdateFile();
		if(Next == NULL) 
			break;	
#ifdef DEBUG
		/*if(Next->m_crc == 0) //Full.zip이면 pass 넘 느려
			return RETURN_NEXT;*/
#endif
		m_iDisplayCnt++;
		m_bWaitBeforeNewpac = false;
		for(int i = 0; i < MAX_DOWN_FILE_LOOP; i++)
		{	
			if(i == MAX_DOWN_FILE_LOOP-1)
				return RETURN_ERROR;

			if(!m_bActive) 
				return RETURN_STOP;

			ReturnState eState = UpdateOneFile(Next);
#ifdef SRC_OVERSEAS
			m_iRetry = 0; //3회시도
			CTime cBaseTime = CTime::GetCurrentTime();
			cBaseTime += CTimeSpan(0,0,0,5); //5초

			while( eState == RETURN_RETRY )
			{
				bool bWait = true;
				while( bWait )
				{
					if( CTime::GetCurrentTime() >= cBaseTime )
					{
						eState = UpdateOneFile(Next);
						bWait = false;
					}
				}
				cBaseTime = CTime::GetCurrentTime();
				cBaseTime += CTimeSpan(0,0,0,5); //5초
			}
#endif

			if(eState == RETURN_STOP)
				return RETURN_STOP;
			else if(eState == RETURN_NEXT)
			{
				break;
			}
			else if(eState == RETURN_ERROR)
			{
				char szTempMsg[MAX_PATH*4]="";
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),i+1, MAX_DOWN_FILE_LOOP );
				SetText(szTempMsg);
				Sleep(500);
				continue;
			}
		}
	}

	return RETURN_NEXT;
}

bool HTTPManager::PieceFileCheck( IN const CString &rszDownloadedFullPath , IN DWORD dwRemoteFileSize, OUT DWORD &rdwDownloadedSize )
{
	if( !Help::IsPieceFilePermit() ) 
		return false;

	CFile file;
	CFileException ex;
	if( !file.Open( (LPCTSTR)rszDownloadedFullPath, CFile::modeRead | CFile::shareDenyNone, &ex ) )
	{
		return false;
	}

	if(rszDownloadedFullPath != "delete_file_list.cfg")
	{
		if( (DWORD)file.GetLength() != dwRemoteFileSize )
		{
			file.Close();
			return false;
		}
	}
	rdwDownloadedSize = (DWORD)file.GetLength();
	file.Close();
	return true;
}

HTTPManager::ReturnState HTTPManager::UpdateOneFile( const PATCHINFOFILE * pNext )
{
	if(pNext == NULL) 
		return RETURN_NEXT;	

	if(pNext->m_szRemoteFilePath.GetLength() <= 3)
		return RETURN_NEXT;

	CString szDownPath = "losadown/";
	szDownPath += pNext->m_szRemoteFilePath;
	CString szDownloadedFullPath;
	Help::SetFullPath( m_szRootDir, szDownPath, szDownloadedFullPath);

	CString szURL = m_szPatchURL;
	szURL += pNext->m_szRemoteFilePath;
#ifdef SRC_OVERSEAS //다운로드 url 뒤에 인자를 붙인다 ?234234(랜덤 수)
	ULONG uRand = 0;
	srand((unsigned)time(NULL));
	uRand=( ( (long) rand()<<15) | rand() ) % 1000000;
	szURL += "?";
	char szTemp[MAX_PATH] = "";
	itoa(uRand , szTemp, 10);
	szURL += szTemp;
#endif
	DWORD dwDownloadedSize = 0;
	
	if( !PieceFileCheck( szDownloadedFullPath, pNext->m_file_size, dwDownloadedSize ) )
	{
		switch(DownLoadProcess(szURL, pNext->m_file_size, szDownloadedFullPath, dwDownloadedSize))
		{
		case RETURN_ERROR:
			{
				static bool isShow = false;
				if( isShow == false)
				{
					isShow = true;
					CString szMsg = STR(1);
					szMsg += m_szErrorMsg; 
					ShowErrorDlg((LPCTSTR)szMsg, 218, false);
					char szText[MAX_PATH] = "";
					StringCbPrintf( szText, sizeof( szText ), "[Fail Download] url : %s , size : %d ", szURL, dwDownloadedSize );
					SendLOG( szText, 218);
				}
			}
			return RETURN_ERROR;
		case RETURN_STOP:
			return RETURN_STOP;
		}
	}
	else
	{
		AddCurrentGauge( dwDownloadedSize );
		LOG.PrintTimeAndLog( 0, "PieceFile Ok %d", m_file_count );
	}

	if( !Help::CheckDownloadedFile(szDownloadedFullPath, m_szErrorMsg ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), Help::GetAnnounceFileName(pNext->m_szLocalFilePath) );
		SetText(szTempMsg);

		static bool isShow = false;
		if( isShow == false)
		{
			isShow = true;
			CString szMsg = STR(3);
			szMsg += m_szErrorMsg; 
			ShowErrorDlg((LPCTSTR)szMsg ,216, false);
			char szText[MAX_PATH] = "";
			StringCbPrintf( szText, sizeof( szText ), "[Damage File] : %s", szDownloadedFullPath );
			SendLOG(szText, 216);
		}
		return RETURN_ERROR;
	}

	if(pNext->m_szLocalFilePath != "delete_file_list.cfg")
	{
		if(pNext->m_file_size != dwDownloadedSize)
		{
			char szTempMsg[MAX_PATH*4]="";
			StringCbPrintf( szTempMsg, sizeof( szTempMsg ), STR(4), Help::GetAnnounceFileName(pNext->m_szLocalFilePath) );
			SetText(szTempMsg);

			DWORD dwCRC = 0;
			CCrc32Static::FileCrc32Streams((LPCTSTR)szDownloadedFullPath, dwCRC);

			StringCbPrintf( szTempMsg, sizeof( szTempMsg ), STR(5), (LPCTSTR) szDownloadedFullPath, pNext->m_file_size, dwDownloadedSize, pNext->m_crc, dwCRC );
			m_szErrorMsg = szTempMsg;
			LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR) m_szErrorMsg);

			// 경고메세지는 한번만 프로세스는 계속진행함.
			static bool isShow = false; 
			if(isShow == false )
			{
				isShow = true;
				CString szMsg = STR(6);
				szMsg += m_szErrorMsg; 
				ShowErrorDlg((LPCTSTR)szMsg ,202, false);
				char szText[MAX_PATH] = "";
				StringCbPrintf( szText, sizeof( szText ), "[File Size is not matched]info : %d | down : %d ", (INT)pNext->m_file_size, (INT)dwDownloadedSize );
				SendLOG(szText, 202);

			}
		}
	}
	if( IsPutPacFile(pNext->m_eFileType))
	{
		if(!AddPacFile(pNext,szDownloadedFullPath))
		{
			if( m_iRetry < 3 )
			{
				++m_iRetry;
				return RETURN_RETRY;
			}
			char szTemp[MAX_PATH]="";
			StringCbPrintf(szTemp, sizeof(szTemp), STR(7) ,(int)pNext->m_file_size,(int)dwDownloadedSize,(int)pNext->m_crc);
			m_szErrorMsg += szTemp;

			static bool isShow = false; 
			if(isShow == false )
			{
				isShow = true;
				CString szMsg = STR(8);
				szMsg += m_szErrorMsg; 
				ShowErrorDlg((LPCTSTR)szMsg ,219, false);
				char szText[MAX_PATH] = "";
				StringCbPrintf( szText, sizeof( szText ), "[Fail Pac Add] type : %d, Path : %s", pNext->m_eFileType, pNext->m_szLocalFilePath );
				SendLOG(szText, 219);
			}
			return RETURN_ERROR;
		}

		m_dwAddPacSize += pNext->m_file_size;
	}
	else
	{
		CString szClientFullPath;
		Help::SetFullPath( m_szRootDir, pNext->m_szLocalFilePath, szClientFullPath);

		if( pNext->m_eFileType != FT_FULLZIP)
		{
			if(!UnPacPieceFile( szDownloadedFullPath ))//IOP패킹을 풀어준다
			{
				char szTemp[MAX_PATH]="";
				StringCbPrintf(szTemp, sizeof(szTemp), STR(9),(int)pNext->m_file_size,(int)dwDownloadedSize,(int)pNext->m_crc);
				m_szErrorMsg += szTemp;

				static bool isShow = false; 
				if(isShow == false )
				{
					isShow = true;
					CString szMsg = STR(10);
					szMsg += m_szErrorMsg; 
					ShowErrorDlg((LPCTSTR)szMsg ,220, false);
					char szText[MAX_PATH] = "";
					StringCbPrintf( szText, sizeof( szText ), "[Fail unpac piece file] type : %d, Path : %s", pNext->m_eFileType, pNext->m_szLocalFilePath );
					SendLOG( szText, 220);
				}
				return RETURN_ERROR;
			}

			// change name for file copy
			szDownloadedFullPath = szDownloadedFullPath.Left( szDownloadedFullPath.GetLength() - 4);

			switch(FileCopy(pNext,szClientFullPath, szDownloadedFullPath))
			{
			case RETURN_ERROR:
				{
					char szTemp[MAX_PATH]="";
					StringCbPrintf(szTemp, sizeof(szTemp), STR(11), (int)pNext->m_file_size, (int)dwDownloadedSize, (int)pNext->m_crc );

					m_szErrorMsg += szTemp;

					static bool isShow = false; 
					if(isShow == false )
					{
						isShow = true;
						CString szMsg = STR(12);
						szMsg += m_szErrorMsg; 
						ShowErrorDlg((LPCTSTR)szMsg ,221, false);
						char szText[MAX_PATH] = "";
						StringCbPrintf( szText, sizeof( szText ), "[Fail copying file] type : %d, Path : %s", pNext->m_eFileType, pNext->m_szLocalFilePath );
						SendLOG( szText, 221);
					}
				}
				return RETURN_ERROR;
			case RETURN_STOP:
				return RETURN_STOP;
			}
		}
		else // full.zip
		{

			switch(SetupFull(szDownloadedFullPath, szClientFullPath))
			{
			case RETURN_ERROR:
				{
					char szTemp[MAX_PATH]="";
					SafeSprintf(szTemp, sizeof(szTemp), STR(13), (int)pNext->m_file_size, (int)dwDownloadedSize );
					m_szErrorMsg += szTemp;

					static bool isShow = false;
					if( isShow == false)
					{
						isShow = true;
						CString szMsg = STR(14);
						szMsg += m_szErrorMsg;
						ShowErrorDlg((LPCTSTR)szMsg ,217, false);
						SendLOG( "Fail full zip", 217);
					}
				}
				return RETURN_ERROR;
			case RETURN_STOP:
				return RETURN_STOP;
			}

			SetTotalDownloadGauge();
		}
	}

	return RETURN_NEXT;
}

HTTPManager::ReturnState HTTPManager::DownLoadProcess( const CString &szURL,
													   int iRemoteFileSize,
													   const CString &rszDownloadedFullPath,
													   OUT DWORD &rdwDownloadedSize )
{
	//char szTempMsg[MAX_PATH*4]="";
	//SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), m_iDisplayCnt );
	//SetText(szTempMsg);

	int iCnt	= 0;
	while(true)
	{
		CString szErrorMsg;
		if( !m_HTTP.OpenUrl(szURL, szErrorMsg) )
		{
			if(iCnt >= 2)
			{
				int	iResult = MessageBox( GetDesktopWindow(), "Sorry. server is full.\nPlease after a while retry again.", "Server Busy", MB_RETRYCANCEL | MB_ICONWARNING | MB_TOPMOST);
				if( IDRETRY == iResult )
					continue;
				else 
					return RETURN_ERROR;
			}
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),Help::GetAnnounceFileName(rszDownloadedFullPath) );
			//SetText(szTempMsg);

			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3),(LPCTSTR)szURL, (LPCTSTR) szErrorMsg );
			m_szErrorMsg = szTempMsg;
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
			m_HTTP.DeleteSession();
			++iCnt;
			continue;
		}
		else
 			break;
	}

	
	CInternetFile *pIF = m_HTTP.GetInternetFile(); 

	if(!pIF)
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4),Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(5),(LPCTSTR)szURL, GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
		m_HTTP.DeleteSession();
		MessageBox(NULL, "pIF Pointer failed", "Error", MB_OK);
		return RETURN_ERROR;
	}

	char szDrive[MAX_PATH]= "";
	char szDir[MAX_PATH*2]= "";
	char szDriveDir[MAX_PATH*2]= "";
	::_splitpath((LPCTSTR)rszDownloadedFullPath,szDrive,szDir,NULL,NULL);
	StringCbCopy( szDriveDir , sizeof( szDriveDir ), szDrive );
	StringCbCat( szDriveDir , sizeof( szDriveDir ), szDir );
	Help::SetCreateDirectoryByFullPath( szDriveDir );

	if(!Help::DeleteFileRepeat( (LPCTSTR) rszDownloadedFullPath ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(6),(LPCTSTR)rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;
	}

	CFile file;
	CFileException ex;
	
	if(!file.Open((LPCTSTR)rszDownloadedFullPath,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite, &ex))
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(7), Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		m_szErrorMsg = STR(8);
		m_szErrorMsg += rszDownloadedFullPath;
		m_szErrorMsg += "\r\n";
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		m_szErrorMsg += szError;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;	
	}
 	m_bShowGauge = true;
	DWORD dwRemoteSize = 0;
	CString strFilename = rszDownloadedFullPath;
	strFilename = strFilename.Right(strFilename.GetLength() - strFilename.ReverseFind('\\')-1);
	DWORD write_file_size = 0;

	if(m_bShowGauge)
	{
		SetMaxGauge( 100 );
		dwRemoteSize = 	m_HTTP.GetFileSize();	
		
		if(strFilename == "full.zip")
		{
			int iLastPercent = -1;
			int iData = file.GetLength();
			CString strServer, strObj;
			INTERNET_PORT nPort;
			DWORD dwServiceType;
			AfxParseURL(szURL,dwServiceType,strServer,strObj,nPort);
			CHttpConnection* pCon = m_HTTP.GetSession()->GetHttpConnection((LPCSTR)strServer);						
			CHttpFile* pHtfile = pCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, (LPCSTR)strObj,NULL, 0, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);
			char szTempMsg[MAX_PATH*4]="";
			itoa(iData,szTempMsg,10);
			CString REQUEST_HEADER = "Accept: */*\r\nUser-Agent: HttpDownload\r\nRange:bytes= ";
			REQUEST_HEADER +=szTempMsg;
			REQUEST_HEADER += "-\r\n";			
			pHtfile->AddRequestHeaders(REQUEST_HEADER);
			try
			{
				if(!pHtfile->SendRequest())
				{
					char szTempMsg[MAX_PATH*4]="";
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4),Help::GetAnnounceFileName(rszDownloadedFullPath) );
					SetText(szTempMsg);

					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(5),(LPCTSTR)szURL, GetLastError() );
					m_szErrorMsg = szTempMsg;
					LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
					m_HTTP.DeleteSession();
					MessageBox(NULL, "Send Request Fail!", "Error", MB_OK);
					return RETURN_ERROR;
				}
			}
			catch( CInternetException *e )
			{
				//"|EXE_HTTPManager::DownLoadProcess_14|full.zip 이어받기 실패. HTTP서버에서 이어받기를 지원하지 않습니다.|"
				m_szErrorMsg = STR(14);
				//m_szErrorMsg += rszDownloadedFullPath;
				//m_szErrorMsg += "\r\n";
				SetText(rszDownloadedFullPath);
				if(e)
				{
					TCHAR szError[1024];
					e->GetErrorMessage(szError, 1024);
					m_szErrorMsg += szError; 
					e->Delete();
				}

				LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

				file.Close();
				m_HTTP.DeleteInternetFile();
				m_HTTP.DeleteSession();
				return RETURN_ERROR;
			}

			pIF = ((CInternetFile*)pHtfile);
			file.Seek(iData,CFile::begin);
			write_file_size = iData;
			int iLogPercent = (int)(write_file_size*100.0f / (float)iRemoteFileSize);
			LOG.PrintTimeAndLog(0,"시작%% : %d", iLogPercent);
			DWORD dwStartTime = timeGetTime();
			
			if(dwRemoteSize != iData)
			{
				static char buf[4096];
				memset( buf, 0, sizeof(buf) );

				int iLastPercent = -1;
				DWORD read_file_cnt = pIF->Read( buf, sizeof(buf) );
				while(read_file_cnt)
				{
					write_file_size += read_file_cnt;
					file.Write((LPCSTR)buf,read_file_cnt);

					int iPercent = 0;
					if( iRemoteFileSize > 0 )
					{
						iPercent = (int)(write_file_size*100.0f / (float)iRemoteFileSize);

						if( m_bShowGauge)
							SetCurrentGauge( iPercent );
					}
					else if( dwRemoteSize > 0 )
					{
						iPercent = (int)(write_file_size*100.0f / (float)dwRemoteSize);
						if( m_bShowGauge)
							SetCurrentGauge( iPercent );
					}
					iPercent = max( 0, min( iPercent, 100 ) );
					char szTempMsg[MAX_PATH*4]="";
					//SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9), m_iDisplayCnt, iPercent );
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9), strFilename, iPercent );
					//LOG.PrintTimeAndLog(0,"Full.zip Downloading Percent : %d ", iPercent);
					if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
						SetText(szTempMsg);
					iLastPercent = iPercent;

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
			}
			file.Close();
			m_HTTP.DeleteInternetFile();
			m_HTTP.DeleteSession();
			rdwDownloadedSize = write_file_size;
			DWORD dwEndTime = timeGetTime();
			Help::MsToTime(LOG,dwEndTime-dwStartTime);
			LOG.PrintTimeAndLog(0,"Downloading Time : %d ms", dwEndTime-dwStartTime);
			return RETURN_NEXT;
		}
	}
	try
	{
		static char buf[4096];
		memset( buf, 0, sizeof(buf) );

		int iLastPercent = -1;
		DWORD read_file_cnt = pIF->Read( buf, sizeof(buf) );
		while(read_file_cnt)
		{
			write_file_size += read_file_cnt;
			file.Write((LPCSTR)buf,read_file_cnt);
			
			int iPercent = 0;
			// 파일 하나의 진행률 계산 및 표시 (아래 두개의 if문)
			//if( iRemoteFileSize > 0 )
			//{
			//	iPercent = (int)(write_file_size*100.0f / (float)iRemoteFileSize);
			//
			//	if( m_bShowGauge)
			//		SetCurrentGauge( iPercent );
			//}
			//else if( dwRemoteSize > 0 )
			//{
			//	iPercent = (int)(write_file_size*100.0f / (float)dwRemoteSize);
			//	if( m_bShowGauge)
			//		SetCurrentGauge( iPercent );
			//}

			// 다운받는 파일 전체 진행률 계산 및 표시
			if(!m_vFileList.empty())
			{
				iPercent = (int)(m_file_count*100.0f / (float)m_vFileList.size());

				if( m_bShowGauge)
					SetCurrentGauge( iPercent );
			}

			iPercent = max( 0, min( iPercent, 100 ) );
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9), "", iPercent );
			LOG.PrintTimeAndLog(0,"Downloading Progress : %d / %d", m_file_count, m_vFileList.size());
			if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
				SetText(szTempMsg);
			iLastPercent = iPercent;

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
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(10),Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		m_szErrorMsg = STR(11);
		m_szErrorMsg += rszDownloadedFullPath;
		m_szErrorMsg += "\r\n";

		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;
	}
	catch( CInternetException *e )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(12),Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		m_szErrorMsg = STR(13);
		m_szErrorMsg += rszDownloadedFullPath;
		m_szErrorMsg += "\r\n";

		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return RETURN_ERROR;
	}

	m_HTTP.DeleteInternetFile();
	m_HTTP.DeleteSession();
	rdwDownloadedSize = write_file_size;
	LOG.PrintTimeAndLog(1, "Complete - download - url:%s - path:%s", (LPCTSTR)szURL, (LPCTSTR)rszDownloadedFullPath);
	LOG.PrintTimeAndLog(0, "Complete - download - %d", m_file_count );
	return RETURN_NEXT;
}

const HTTPManager::PATCHINFOFILE * HTTPManager::GetUpdateFile( bool bShowPercent /*= false*/, bool bCheckOverlapFile /*= false*/  )
{
#ifdef _DEBUG
//#if 1 
	//LOG.PrintTimeAndLog( 0, "DEBUG Mode No Patch.");
	//return NULL;
#endif
	
	//if( !bShowPercent )
	//	SetText(STR(1));

	if(m_vFileList.empty()) return NULL;
	int iSize = m_vFileList.size();
	if(m_file_count >= iSize) return NULL;
	
	int iLastPercent = -1;
	for(int i = m_file_count;i < iSize;i++)
	{
		if(!m_bActive)
			return NULL;

		//if( bShowPercent )
		//{
		//	int iPercent = (int)((i+1)*100.0f / (float)iSize);
		//	char szTempMsg[MAX_PATH*4]="";
		//	SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), iPercent );
		//	if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
		//		SetText(szTempMsg);
		//	iLastPercent = iPercent;
		//}

		CString szClientPath = m_vFileList[i].m_szLocalFilePath;
		bool is_change = false;
		DWORD dwCRC=0;
		int   iUnZipFileSize=0;
		int	  iTempIndex = 0;
		int	  iPacArray = 0; //iArray(팩파일 번호)내용 저장영
		if(IsPutPacFile(m_vFileList[i].m_eFileType))
		{
			// change name
			CString szInPacPath = szClientPath;
			szInPacPath.Replace('/', '\\');

			int iArray = IsMapPacFile( m_vFileList[i].m_eFileType );

			int iIndex = g_PacChild(iArray)->GetFileIndex(szInPacPath);
			if(iIndex == -1)
				is_change = true;
			else
			{
				iTempIndex = iIndex;
				iPacArray = iArray;
				dwCRC = g_PacChild(iArray)->GetFileCRC( iIndex );
				iUnZipFileSize = g_PacChild(iArray)->GetFileSize( iIndex );
			}
		}
		else
		{
			CString szFullPath;
			Help::SetFullPath( m_szRootDir, szClientPath, szFullPath);

			CFile file;
			if(!file.Open((LPCTSTR)szFullPath,CFile::modeRead | CFile::shareDenyNone))//리턴값 0=열기에 실패 이곳에 full.zip이 없을때 새로받는데. 받는 중간이면 무조건없을건데 full.zip 검사
				is_change = true;
			else
			{
				iUnZipFileSize = (int) file.GetLength();	
				CCrc32Static::FileCrc32Streams((LPCTSTR)szFullPath, dwCRC);
				
#ifdef _ADMIN_ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				CString szCheck = szFullPath;
				szCheck.MakeLower();
				if( !szCheck.Find( "tool/") )
				{
					CString szTemp = szFullPath;
					szTemp.MakeLower();
					szTemp.MakeReverse();
					if( szTemp.Find("ini." ) == 0 )
					{
						char *pData = new char[iUnZipFileSize];
						if( file.Read( pData, iUnZipFileSize ) == iUnZipFileSize )
						{
							ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
							if( pLocal )
							{
								pLocal->EncryptDecryptData( pData, iUnZipFileSize, pData, iUnZipFileSize, false );
								CCrc32Static::MemoryCrc32( pData, iUnZipFileSize, dwCRC );
							}
						}	
						delete [] pData;
					}
				}
#endif /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				file.Close();
			}
		}

		if(!is_change)
		{
			if( dwCRC		     != m_vFileList[i].m_crc ||
				iUnZipFileSize   != m_vFileList[i].m_unzip_file_size )
			{
				is_change = true;

				// 변경하지 않음. 
				// 로컬 full.zip 파일은 버전 정보 파일로써 파일이 없는 경우에만 full.zip 파일을 다시 다운로드 한다.
				// ActiveX에서 버전 정보를 확인해서 버전정보가 다른 경우 full.zip 파일을 삭제함.
				if( m_vFileList[i].m_eFileType == FT_FULLZIP || m_vFileList[i].m_eFileType == FT_AUTOUPGRADE)
					is_change = false;
			}
		}
//#ifdef SHIPPING
		//맨처음 파일만 확인하고 full.zip을 다운받을 경우 중복파일 제거 안함
		if( i < 1 && is_change && szClientPath == CString("full.zip") ) 
		{
			m_bOverlapCheck = false;
		}		
		else if(bCheckOverlapFile)
			m_bOverlapCheck = true;
//#endif


		// 패치하지 않음  테스트용
		// 	if( szClientPath.Find("lostsaga.exe")    != -1  || 
		// 	    szClientPath.Find("autoupgrade.exe") != -1  || 
		// 		szClientPath.Find("io3dengine.dll")  != -1  ||
		// 		szClientPath.Find("ioprotect.exe")   != -1  ||
		// 	    szClientPath.Find("iopac.dll")       != -1  ||
		// 		szClientPath.Find("townportal.dll")  != -1  ||  
		// 		szClientPath.Find("iofreetype.dll")  != -1
		// 		)
		// 		continue;
//#ifdef SHIPPING //_DEBUG
//#ifdef _DEBUG
		if( !bShowPercent && is_change && bCheckOverlapFile)
		{
			m_bOverlapCheck;
			if(m_bOverlapCheck)
			{
				Overlapdata Odata; //다운받은 파일 정보
				if( m_vFileList[i].m_crc != 0 ) //0이면 못받는 파일 혹은 신규파일입니다.
				{
					Odata.first  = m_vFileList[i].m_szLocalFilePath; 
					Odata.second = m_vFileList[i].m_crc;
					m_vecOverlap[iPacArray].push_back(Odata);
				}
			}
		}
//#endif	
		if(is_change)
		{
			m_file_count = i + 1;
			return &m_vFileList[i];
		}
		
	}
	return NULL;
}

HTTPManager::ReturnState HTTPManager::FileCopy( const PATCHINFOFILE *pNext, const CString &rszClientFullPath, const CString &rszDownloadedFullPath )
{
#ifdef SHIPPING
	if( pNext->m_eFileType != FT_AUTOUPGRADE && pNext->m_eFileType != FT_FULLZIP)     
	{		
#else
	if( ( pNext->m_eFileType != FT_AUTOUPGRADE || !m_bAdminPatch ) && pNext->m_eFileType != FT_FULLZIP)
	{
#endif
		char dir[MAX_PATH]		= "";		
		::_splitpath((LPCTSTR)pNext->m_szLocalFilePath,NULL,dir,NULL,NULL);
		Help::SetCreateDirectory(m_szRootDir, dir);

		if(!Help::MoveFileRepeat( (LPCTSTR) rszDownloadedFullPath, (LPCTSTR) rszClientFullPath ) )
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), Help::GetAnnounceFileName(pNext->m_szLocalFilePath)  );
			SetText(szTempMsg);

			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),(LPCTSTR)rszDownloadedFullPath,(LPCTSTR)rszClientFullPath, (int)GetLastError() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
			return RETURN_ERROR;
		}
	}
	
	LOG.PrintTimeAndLog(0, "Complete move file.");
#ifdef _ADMIN_
	if(m_bAdminPatch == false)
		return RETURN_NEXT;

	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg->IsCopyFolder())
		return RETURN_NEXT;
#endif

	return RETURN_NEXT;
}


HTTPManager::ReturnState HTTPManager::UnFull( const CString &rszDownloadedFullPath )
{
	//full.zip 풀어주는 부분
	if( !g_PacChild(FULL_PAC) )
	{
		SetText(STR(1));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
		return RETURN_ERROR;
	}
	// 압축 풀어주는 부분
	// 풀집은 암호가 없다.
	char szEncPW[MAX_PASSWORD+1]="";
	if(!g_PacChild(FULL_PAC)->OpenBigPac((LPCTSTR)rszDownloadedFullPath, true, false, szEncPW ))
	{
		SetText(STR(3));
		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4), (LPCTSTR)rszDownloadedFullPath, GetLastError() ,dwCRC, PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
		return RETURN_ERROR;
	}

	int iLastPercent = -1;
	int iMax = g_PacChild(FULL_PAC)->GetMaxIndex();
	for(int i = 0; i < iMax; i++)
	{
		if(!m_bActive)
		{
			g_PacChild(FULL_PAC)->CloseBigPac();
			return RETURN_STOP;
		}

		char szZipPath[MAX_PATH]="";
		if(!g_PacChild(FULL_PAC)->GetFileName(i, szZipPath, MAX_PATH-1))
		{
			SetText(STR(5));
			DWORD dwCRC = 0;
			CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(6), (LPCTSTR)rszDownloadedFullPath, GetLastError() ,dwCRC, i, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
			return RETURN_ERROR;
		}

		if(!strcmp(szZipPath, "This is a Directory!!!!"))
			continue;

		int iPercent = (int)((i+1)*100.0f / (float)iMax);
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(7), m_iDisplayCnt, iPercent );
		if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
			SetText(szTempMsg);
		iLastPercent = iPercent;

		char szFileName[MAX_PATH]="";
		_splitpath(szZipPath, NULL, NULL, szFileName, NULL);

		if( strcmp( szFileName, "autoupgrade") == 0 ) // 현재 실행중이므로 압축을 풀 수 없다.
			continue;
				
		CString szFullPath = m_szRootDir;
		szFullPath += "\\";
		szFullPath += szZipPath;
		if(!Help::DeleteFileRepeat( (LPCTSTR) szFullPath ) )
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(8),(LPCTSTR) szFullPath, GetLastError() );
			m_szErrorMsg = szTempMsg;
			LOG.PrintTimeAndLog(0, "%s" ,m_szErrorMsg);
			return RETURN_ERROR;
		}

		if(!g_PacChild(FULL_PAC)->Extract(i,""))
		{
			szFileName[20]=NULL;//20자까지만 표시

			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9), szFileName );
			SetText(szTempMsg);

			DWORD dwCRC = 0;
			CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(10),(LPCTSTR) szFullPath ,GetLastError(), dwCRC,  i, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;
			LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
			return RETURN_ERROR;
		}
	}
	
	g_PacChild(FULL_PAC)->CloseBigPac();

	if( ! Help::DeleteFileRepeat( (LPCTSTR ) rszDownloadedFullPath ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(11),(LPCTSTR ) rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return RETURN_ERROR;
	}

	return RETURN_NEXT;
}

bool HTTPManager::CheckPac( const char* szFullPath, const int iDisplayCnt , const int iMaxDisplayCnt )
{
	if( !g_PacChild(CHECK_PAC) )
	{
		SetText(STR(1));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
		return false;
	}

	char szPacPassword[MAX_PASSWORD+1]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, 0 );
	// 처음 접속 유저인 경우 파일이 없기때문에 오픈에 실패할 수 있다.
	if(!g_PacChild(CHECK_PAC)->OpenBigPac(szFullPath, true, false, szPacPassword ))
		return true;

	int iLastPercent = -1;
	int iMax = g_PacChild(CHECK_PAC)->GetMaxIndex();
	for(int i = 0; i < iMax; i++)
	{
		if(!m_bActive)
		{
			g_PacChild(CHECK_PAC)->CloseBigPac();
			return false;
		}

		int iPercent = (int)((i+1)*100.0f / (float)iMax);
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), iDisplayCnt, iMaxDisplayCnt, iPercent );
		if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
			SetText(szTempMsg);
		iLastPercent = iPercent;

		if(!g_PacChild(CHECK_PAC)->TestFile(i))
		{
			char szZipPath[MAX_PATH]="";
			g_PacChild(CHECK_PAC)->GetFileName(i, szZipPath, MAX_PATH-1);
			LOG.PrintTimeAndLog(0, "%d번째 파일 검사중 에러 파일명:%s", iDisplayCnt, szZipPath );
			
			g_PacChild(CHECK_PAC)->CloseBigPac();
			return false;
		}
	}

	g_PacChild(CHECK_PAC)->CloseBigPac();
	LOG.PrintTimeAndLog(0, "Complete check pac");
	return true;
}

bool HTTPManager::UnPac( const char* szPacName, const int iDisplayCnt , const int iMaxDisplayCnt )
{
#ifdef _ADMIN_
	LOG.PrintTimeAndLog(0, "Start unpac.");
	if( !g_PacChild(RS_PAC) )
	{
		SetText(STR(1));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), szPacName, GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;		
	}

	char szPacPassword[MAX_PASSWORD+1]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );
	if(!g_PacChild(RS_PAC)->OpenBigPac(szPacName, true, true, szPacPassword ) )
	{
		SetText(STR(3));

		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4), szPacName, GetLastError() ,PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;		
	}

	if(!g_PacChild(RS_PAC)->InitSetIter())
	{
		SetText(STR(5));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(6), szPacName, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	int iLastPercent = -1;
	int iMax = g_PacChild(RS_PAC)->GetSetSize();
	for (int i = 0; i < iMax ; i++)
	{
		if(!m_bActive)
			break;

		FileInfo kFileInfo;
		if(!g_PacChild(RS_PAC)->GetCurFileInfo(kFileInfo))
		{
			SetText(STR(7));
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(8), szPacName, GetLastError(), i, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;
			
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
			return false;
		}

		int iPercent = (int)((i+1)*100.0f / (float)iMax);
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9), iDisplayCnt, iMaxDisplayCnt , iPercent );
		if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
			SetText(szTempMsg);
		iLastPercent = iPercent;

		char szFileName[MAX_PATH]="";
		_splitpath(kFileInfo.FileName.c_str(), NULL, NULL, szFileName, NULL);
		if( strcmp( szFileName, "autoupgrade") == 0 ) // 현재 실행중이므로 압축을 풀 수 없다.
		{
			g_PacChild(RS_PAC)->NextSetIter();
			continue;
		}

		CString szFullPath = m_szRootDir;
		szFullPath += "\\";
		szFullPath += kFileInfo.FileName.c_str();
		if(!Help::DeleteFileRepeat( (LPCTSTR) szFullPath) )
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(10), szFullPath, GetLastError() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
			return false;
		}

		if(!g_PacChild(RS_PAC)->Extract(""))
		{
			SetText(STR(11));
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(12), kFileInfo.FileName.c_str(), kFileInfo.HashCode, kFileInfo.Index, kFileInfo.ZipFileSize, GetLastError(), i, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
			return false;
		}

		char szComment[MAX_PATH]="";
		g_PacChild(RS_PAC)->GetFileComment( szComment, sizeof( szComment ), kFileInfo.Index );
		CString szTemp = szFullPath;
		szTemp.MakeLower();
		szTemp.MakeReverse();

		if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
			DecryptFile( szFullPath );

		g_PacChild(RS_PAC)->NextSetIter();
	}
	
	g_PacChild(RS_PAC)->CloseBigPac();

	LOG.PrintTimeAndLog(0, "Complete unpac.");
#endif 
	return true;
}

bool HTTPManager::AddPacFile( const PATCHINFOFILE *pFile,const CString &rszDownloadedFullPath )
{
	// change name
	CString szInPacPath = pFile->m_szLocalFilePath;
	szInPacPath.Replace('/', '\\');

	// pac target
	int iArray = IsMapPacFile( pFile->m_eFileType );

	//char szTempMsg[MAX_PATH*4]="";
	//SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), m_iDisplayCnt );
	//SetText(szTempMsg);
	
	char szComment[MAX_PATH]="";
	g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)rszDownloadedFullPath );
	char szPacPassword[MAX_PASSWORD+1]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );

	if(!g_PacChild(iArray)->AddBigPac((LPCTSTR)rszDownloadedFullPath, (LPCTSTR)szInPacPath , time(NULL), szPacPassword, szComment ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), (LPCTSTR)rszDownloadedFullPath,(LPCTSTR)szInPacPath, iArray, dwCRC, GetLastError(), PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	if( ! Help::DeleteFileRepeat( (LPCTSTR ) rszDownloadedFullPath ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4),(LPCTSTR ) rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return false;
	}
	//config/mercenary/179/179_extraitem.ini
	LOG.PrintTimeAndLog(1, "Complete - AddBigPac - %s - %s", (LPCTSTR)rszDownloadedFullPath, (LPCTSTR)szInPacPath);
	LOG.PrintTimeAndLog(0, "Complete - AddBigPac - %d", m_file_count );
	return true;	
}

bool HTTPManager::IsPutPacFileByPath( CString szRemoteFilePath )
{
	szRemoteFilePath.MakeLower();

	if(!szRemoteFilePath.Find("config/"))
		return true;
	if(!szRemoteFilePath.Find("resource/"))
		return true;
	if(!szRemoteFilePath.Find("scripts/"))
		return true;
	if(!szRemoteFilePath.Find("shader/"))
		return true;
	if(!szRemoteFilePath.Find("xml/"))
		return true;
	if(!szRemoteFilePath.Find("_map/"))
		return true;
#ifdef SRC_EU
	if(!szRemoteFilePath.Find("_map_en/"))
		return true;
	if(!szRemoteFilePath.Find("_map_de/"))
		return true;
	if(!szRemoteFilePath.Find("_map_fr/"))
		return true;
	if(!szRemoteFilePath.Find("_map_it/"))
		return true;
	if(!szRemoteFilePath.Find("_map_pl/"))
		return true;
	if(!szRemoteFilePath.Find("_map_tr/"))
		return true;
#endif
	return false;
}

int HTTPManager::IsMapPacFileByPath( CString szRemoteFilePath )
{
	szRemoteFilePath.MakeLower();

	if(!szRemoteFilePath.Find("_map/"))
		return FT_PAC_MAP;
#ifdef SRC_EU
	else if(!szRemoteFilePath.Find("_map_en/"))
		return FT_PAC_MAP_EN;
	else if(!szRemoteFilePath.Find("_map_de/"))
		return FT_PAC_MAP_DE;
	else if(!szRemoteFilePath.Find("_map_fr/"))
		return FT_PAC_MAP_FR;
	else if(!szRemoteFilePath.Find("_map_it/"))
		return FT_PAC_MAP_IT;
	else if(!szRemoteFilePath.Find("_map_pl/"))
		return FT_PAC_MAP_PL;
	else if(!szRemoteFilePath.Find("_map_tr/"))
		return FT_PAC_MAP_TR;
#endif

	return FT_PAC_RS;
}

bool HTTPManager::IsPutPacFile( const FileType eFileType )
{

#ifdef _ADMIN_
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg)
	{
		if(pDlg->IsPieceFileUpdate())
			return false;
	}
#endif

	if( eFileType == FT_PAC_RS || eFileType == FT_PAC_MAP || eFileType == FT_PAC_MAP_EN
		|| eFileType == FT_PAC_MAP_DE || eFileType == FT_PAC_MAP_FR || eFileType == FT_PAC_MAP_IT
		|| eFileType == FT_PAC_MAP_PL || eFileType == FT_PAC_MAP_TR )
		return true;

	return false;
}

int HTTPManager::IsMapPacFile( const FileType eFileType )
{
	switch(eFileType)
	{
	case FT_PAC_MAP:
		return MAP_PAC;
	case FT_PAC_MAP_EN:
		return MAP_EN;
	case FT_PAC_MAP_DE:
		return MAP_DE;
	case FT_PAC_MAP_FR:
		return MAP_FR;
	case FT_PAC_MAP_IT:
		return MAP_IT;
	case FT_PAC_MAP_PL:
		return MAP_PL;
	case FT_PAC_MAP_TR:
		return MAP_TR;
	default:
		return RS_PAC;
	}
}

bool HTTPManager::UnPacPieceFile( const CString &rszDownloadedFullPath )
{
	char szTempMsg[MAX_PATH*4]="";
	SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), m_iDisplayCnt );
	SetText(szTempMsg);

	CString szUnpacFilePath = rszDownloadedFullPath;
	szUnpacFilePath = szUnpacFilePath.Left( szUnpacFilePath.GetLength() - 4 );

	if( !Help::DeleteFileRepeat( (LPCTSTR) szUnpacFilePath) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), (LPCTSTR) rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;
		
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return false;
	}
	
	// is exist
	FILE *pFile = fopen( (LPCTSTR)rszDownloadedFullPath , "rt" );
	if( pFile == NULL )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), (LPCTSTR) rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return false;
	}
	fclose( pFile );
	
	CPacUtil::ReturnValue rValue = CPacUtil::RV_OK;

#ifdef SHIPPING
	rValue = g_Pac.UnPacPieceFile((LPCTSTR)rszDownloadedFullPath);
#else
	char szComment[MAX_PATH]="";
	g_Pac.GetPieceFileComment( szComment, sizeof( szComment ), (LPCTSTR)rszDownloadedFullPath );
	char szPacPassword[MAX_PATH]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, atoi( szComment ) );
	rValue = g_Pac.UnPacPieceFileWithPassword((LPCTSTR)rszDownloadedFullPath, szPacPassword );

	CString szTemp = szUnpacFilePath;
	szTemp.MakeLower();
	szTemp.MakeReverse();
	if( szTemp.Find("ini." ) == 0 && atoi( szComment ) == 1 )
		DecryptFile( szUnpacFilePath );
#endif

	if(rValue != CPacUtil::RV_OK)
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4),Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);
		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams((LPCTSTR)rszDownloadedFullPath, dwCRC);
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(5), (LPCTSTR)rszDownloadedFullPath, (int)rValue, dwCRC , GetLastError() , PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
		return false;
	}

	if( !Help::DeleteFileRepeat( (LPCTSTR) rszDownloadedFullPath ) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(6),(LPCTSTR) rszDownloadedFullPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return false;
	}

#ifdef SHIPPING
	LOG.PrintTimeAndLog(0, "Complete - UnPacPieceFile %d", m_file_count );
#else
	LOG.PrintTimeAndLog(0, "Complete - UnPacPieceFile - %s", (LPCTSTR)rszDownloadedFullPath);
#endif
	return true;	
}

void HTTPManager::SetMaxGauge( const int iMax )
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if(pDlg)
	{
		pDlg->m_ProgressBar.SetMaxPosition(iMax);
		pDlg->m_ProgressBar.Invalidate();
		Sleep(1); // 메인스레드가 동작할 수 있도록
	}
}

void HTTPManager::SetCurrentGauge( const int iPos )
{

	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if(pDlg)
	{
		int iPreRate = (int) ( pDlg->m_ProgressBar.GetProgressRate()*1000 );
		pDlg->m_ProgressBar.SetCurPosition(iPos);

		int iCurRate = (int) ( pDlg->m_ProgressBar.GetProgressRate()*1000 );
		if( iPreRate != iCurRate )
		{
			pDlg->m_ProgressBar.Invalidate();
			Sleep(1); // 메인스레드가 동작할 수 있도록
		}
	}
}

void HTTPManager::AddCurrentGauge( const int iPos )
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if(pDlg)
	{
		int iPreRate = (int) ( pDlg->m_ProgressBar.GetProgressRate()*1000 );
		pDlg->m_ProgressBar.SetCurPosition( iPos + pDlg->m_ProgressBar.GetCurPos() );

		int iCurRate = (int) ( pDlg->m_ProgressBar.GetProgressRate()*1000 );
		if( iPreRate != iCurRate )
		{
			pDlg->m_ProgressBar.Invalidate();
			Sleep(1); // 메인스레드가 동작할 수 있도록
		}
	}
}

void HTTPManager::SetFullGauge()
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if(pDlg)
	{
		pDlg->m_ProgressBar.SetCurPosition( pDlg->m_ProgressBar.GetMaxPos() );
		pDlg->m_ProgressBar.Invalidate();
		Sleep(1); // 메인스레드가 동작할 수 있도록
	}
}

void HTTPManager::SetTotalDownloadGauge()
{
	int iMaxGauge = 0;
	int iMaxFile  = m_vFileList.size();
	m_file_count   = 0; // 초기화  
	for (int i = 0; i < iMaxFile ; i++)
	{
		const PATCHINFOFILE *Next = GetUpdateFile( true );
		if(Next == NULL) 
			break;	
		iMaxGauge += Next->m_file_size;
		if(Next->m_eFileType == FT_FULLZIP )
			break;
	}
	SetMaxGauge( iMaxGauge );
	SetCurrentGauge( 0 );
	m_file_count   = 0; // 초기화
}

void HTTPManager::SetText( const char *szHelp )
{
	if( !m_bActive )
		return;
	AfxGetApp()->GetMainWnd()->SetDlgItemText(IDC_TEXT, szHelp);
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg)
		pDlg->m_Static.Invalidate();
}

void HTTPManager::StartGame()
{
	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	//--Check SteamLauncher
#ifdef STEAM_ATTACH
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US ||
		ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL )
	{
		CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if( pDlg )
		{
			if( pDlg->CheckLauncher() )
				return;
		}
	}
#endif

	//--Read to Register Key
	char szRegKey[MAX_PATH*2]="";
	StringCbPrintf( szRegKey , sizeof( szRegKey ), "%sWebBroker", pLocal->GetRegKey() );
	char szPath[MAX_PATH*2]="";
	Help::RegReadString( REG_SHCU, szRegKey, "ClientPath", "",  szPath, sizeof(szPath) );
	if(szPath[0] == NULL) //client path is not registered
	{
		GetCurrentDirectory(MAX_PATH,(LPSTR)szPath);
		Help::RegWriteString( REG_SHCU, szRegKey, "ClientPath", (LPCTSTR)szPath);
	}

	char szNewCmd[4096]="";
	ZeroMemory( szNewCmd, sizeof( szNewCmd ) );
	if( pLocal )
		pLocal->GetNewCmdLine( AfxGetApp()->m_lpCmdLine, szNewCmd, sizeof( szNewCmd ) );

	// wait start
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( pDlg )
	{
		if( pDlg->IsWaitingDlgError() )
		{
			LOG.PrintTimeAndLog(0, "IP is blocked");
			SuspendRun( 1800000 );
			return;
		}

		if( !pDlg->CheckParamVersion( szNewCmd ) )
		{
			MessageBox(NULL, STR(5), STR(6), MB_OK);

			PostQuitMessage(0);

			return;
		}

		pDlg->ShowStartBtn();

		if(pLocal && pLocal->IsAutoStart() )
		{
			char szTempMsg[MAX_PATH*4]="";
 			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), WAIT_TIME_START/1000 );
			SetText( szTempMsg );
			pDlg->SetTimer( TIME_ID_WAIT_START, 1000, NULL );
			SuspendRun( 40000 );
		}
		else
		{
			SetText( STR(4));
			SuspendRun( INFINITE );
		}
	}

	if( !m_bActive )
		return;

	if( pLocal && pLocal->IsLoginDlg() )
	{
		if( pDlg )
		{
			pDlg->ShowLogin();
			pDlg->ShowWindow( SW_HIDE );
			SuspendRun( 1800000 ); // 30분간 대기

			if( !pDlg->IsLoginOk() )
			{
				if( m_bActive )
					pDlg->SetWantEndDialog();
			}

			if( m_bActive )
				pDlg->ShowWindow( SW_SHOW );
		}
	}

#ifndef SRC_TH
	if( pLocal && pLocal->IsOTP() )
	{
		if( pDlg )
		{
			pDlg->ShowOTP();
			pDlg->ShowWindow( SW_HIDE );
			SuspendRun( 1800000 ); // 30분간 대기

			if( !pDlg->IsOTPOk() )
			{
				if( m_bActive )
					pDlg->SetWantEndDialog();
			}

			if( m_bActive )
				pDlg->ShowWindow( SW_SHOW );
		}
	}
#endif
	if( !m_bActive )
		return;

	SetText( STR(1));

	char szClientName[MAX_PATH] = "";
	StringCbPrintf(szClientName, sizeof(szClientName), CLIENT_FILE_NAME,m_szRootDir);

	char szLuncherName[MAX_PATH]="";
	StringCbPrintf(szLuncherName, sizeof(szLuncherName), LAUNCHER_FILE_NAME, m_szRootDir);

	if(pDlg)
	{
		DWORD dwLoginKeyLiveTime = ioLocalParent::LOGIN_KEY_LIVE_TIME;
		if( pLocal )
			dwLoginKeyLiveTime = pLocal->GetLoginKeyLiveTime();

		if( timeGetTime() - pDlg->GetStartTime() >= dwLoginKeyLiveTime )
		{
			LOG.PrintTimeAndLog( 0, "~~~~~Login Key Time Out~~~~~");
			//pDlg->ExcuteProcess(szLuncherName, CMD_RESTART_ADD_SP);
			Help::MessageboxShow(pDlg->GetSafeHwnd(), STR(2),				        
				APP_ERROR_NAME,
				MB_TOPMOST );
			SendLOG("over LOGIN_KEY_LIVE_TIME", 226 );
		}
		else
		{
#ifndef DEBUG
			pDlg->MinimizeAllWindows(true);
#endif
#ifdef XTRAP
			g_ioXtrap.RunPatch();
#endif
			m_pAutoupDlg->SetAutoupStep( CAutoUpgradeDlg::ASP_EXECUTE_LOSA );
#ifdef KAMU_EAC
			pDlg->ExecuteProcess(szLuncherName, szNewCmd);
#else
			pDlg->ExecuteProcess(szClientName, szNewCmd);
#endif

			if( g_LocalMgr.GetLocalType() ==ioLocalManager::LCT_SINGAPORE )
			{
				char szMyIP[MAX_PATH]="";
				Help::GetExternalIP( szMyIP, sizeof( szMyIP ) );

				CString szCheckURL = "http://citystat.playone.asia:8087";
				g_HttpMgr.SendGetData(szCheckURL,szMyIP,false);
			}

			m_eRunEndType = ET_WAIT_CLIENT_MSG;
		}
	}
	else
		LOG.PrintTimeAndLog( 0, "~~~~~DLG_ERR(1)~~~~~");
}

void HTTPManager::ShowErrorDlg( const char* szHelp, int iErrorNumber, bool bExitProgram /*= true*/ )
{
	if(AfxGetApp())
	{
		CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*)AfxGetApp()->GetMainWnd();

		if(pDlg)
			pDlg->ShowErrorDlg(szHelp, iErrorNumber, bExitProgram);
	}
	m_eRunEndType = ET_ERROR_EXIT;
}

void HTTPManager::LoadINI()
{
	if( !AfxGetApp() )
		return;
	if( !AfxGetApp()->GetMainWnd() )
		return;
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( !pDlg )
		return;

	CString szINIPath = "info\\";
	szINIPath += pDlg->GetININame();
	ioINILoaderAU kIniLoder( (LPCTSTR) szINIPath );
	kIniLoder.SetTitle("autoupgrade_info");
	enum { MAX_KEY = 5, };
	const char szKeys[MAX_KEY][MAX_PATH]={"patch_url", "login_url", "shortcut_name", "admin_patch_url", "copy_version" };
	for (int i = 0; i < MAX_KEY ; i++)
	{
		char szBuf[MAX_PATH]="";
		kIniLoder.LoadString(szKeys[i], "", szBuf, sizeof(szBuf));
		switch(i)
		{
		case 0:
			//garena일 경우 마지막 인자값을 통한 패치경로가 변경됩니다.
			if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
			{
				CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
				CString strAddUrl = pDlg->GetCFSourceFolder();
				if(strAddUrl.IsEmpty())
				{
					m_szPatchURL = szBuf;
					break;
				}

				CString tempStr = szBuf;
				const char * cFind = "Client";
				int nStart = tempStr.Find(cFind);
				int nEnd = tempStr.GetLength() - nStart;

				m_szPatchURL.FormatMessage("%1%2/%3", tempStr.Left(nStart), strAddUrl, tempStr.Right(nEnd));
				
				break;
			}
			else
			{
				m_szPatchURL = szBuf;
				break;
			}
		case 1:
			m_szLoginURL = szBuf;
			break;
		case 2:
			m_szShorcutName  = szBuf;
			break;
		case 3:
			m_szAdminPatchURL = szBuf;
			break;
		case 4:
			m_szCopyVersion = szBuf;
			break;
		}
	}
}

bool HTTPManager::_Run()
{
	SetText( STR(1));
//	MessageBoxA(NULL,"HTTPManager::_Run()","Park",MB_OK);
// IE창과 충돌 문제로 제거 ( 클라이언트에서 체크 하므로 )
//	if(!Help::IsRightDXVersion())
//	{
//		ShowErrorDlg(STR(2),211);
//		SendLOG( "Wrong DirectX", 211);
//		return false;
//	}

	if( !ProcessCheckPac() )
	{
		LOG.PrintTimeAndLog( 0, "%s - Fail ProcessCheckPac", __FUNCTION__ );
		return false;
	}

// #ifdef _DEBUG
// 	return true;
// #endif
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	for (int i = 0; i < m_iMaxPac ; i++)
	{
		char szPacPassword[MAX_PASSWORD+1]="";
		
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );
		if(!g_PacChild(i)->OpenBigPac(m_szPacFullPath[i],false, true, szPacPassword ))
		{
			// full.zip 다시 받기
			CString szFullPath = m_szRootDir;
			szFullPath += "\\full.zip";
			if(!Help::DeleteFileRepeat( (LPCTSTR) szFullPath) )
			{
				char szTempMsg[MAX_PATH*4]="";
				StringCbPrintf( szTempMsg, sizeof( szTempMsg ), STR(3), i+1, (LPCTSTR) szFullPath, GetLastError() );
				m_szErrorMsg = szTempMsg;

				ShowErrorDlg( (LPCTSTR) m_szErrorMsg ,222);
				SendLOG( m_szErrorMsg, 222);
				LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
				return false;
			}

			LOG.PrintTimeAndLog(0, "Complete - _Run - ReDownload full.zip - %d", i+1);
		}	
	}

	CString szURL = m_szPatchURL;
	szURL += "server_patch.cfg.iop";

#ifdef SRC_OVERSEAS
	ULONG uRand = 0;
	srand((unsigned)time(NULL));
	uRand=( ( (long) rand()<<15) | rand() ) % 1000000;
	szURL += "?";
	char szTemp[MAX_PATH] = "";
	itoa(uRand , szTemp, 10);
	szURL += szTemp;
#endif

	CString szDownloadedFullPath = m_szRootDir;
	szDownloadedFullPath +="\\";
	szDownloadedFullPath += "losadown\\server_patch.cfg.iop";
	DWORD dwDownloadedFileSize = 0;
	m_iDisplayCnt = 1;

	// 원격지 파일 사이즈를 알 수 없으므로 0으로 설정함.
	switch(DownLoadProcess(szURL, 0, szDownloadedFullPath, dwDownloadedFileSize))
	{
	case RETURN_ERROR:
		{
			CString szMsg = STR(4);
			szMsg += m_szErrorMsg; 
			ShowErrorDlg((LPCTSTR)szMsg, 203);
			SendLOG( "Fail downloading patch info", 203);
		}
		return false;
	case RETURN_STOP:
		return false;
	}
	
	if( !Help::CheckDownloadedFile( szDownloadedFullPath , m_szErrorMsg ) )
	{
		CString szMsg = STR(5);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg,215);
		SendLOG( "Fail damage patch info", 215);
		return false;
	}

	DWORD dwCRC = 0;
	DWORD dwFileSize = 0;
	FILE *pFile = fopen( (LPCTSTR)szDownloadedFullPath , "rt" );
	if( pFile != NULL )
	{
		CCrc32Static::FileCrc32Streams((LPCTSTR)szDownloadedFullPath, dwCRC);
		fseek(pFile, 0, SEEK_END); 
		dwFileSize = ftell(pFile);
		fclose( pFile );
	}

	if(!UnPacPieceFile( szDownloadedFullPath ))
	{
		CString szMsg =STR(6);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg,204);
		//SendLOG( "Fail unpacing patch info", 204);

		char szTempMsg[MAX_PATH*2]="";
		sprintf_s( szTempMsg, sizeof( szTempMsg ), "Fail unpacing patch info - CRC :%d, Size :%d", dwCRC, dwFileSize );
		SendLOG( szTempMsg, 204 );
		return false;
	}

	if(!ParsePatchFile())
	{
		CString szMsg = STR(7);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg,205);
		SendLOG( "Fail reading patch info", 205);
		return false;
	}
	m_pAutoupDlg->SetAutoupStep( CAutoUpgradeDlg::ASP_PATCH_INFO);

	DWORD dwFullZipSize = 0;
	if( !m_vFileList.empty() )
		dwFullZipSize = m_vFileList[0].m_file_size;

	switch(Help::CheckRightHDDSpace( dwFullZipSize , m_szErrorMsg))
	{
	case Help::RV_ANNOUNCE:
		{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(8) , GetLastError() );
		m_szErrorMsg = szTempMsg;
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		ShowErrorDlg((LPCTSTR) m_szErrorMsg,223, false);
		SendLOG( "Fail function to check harddisk", 223);
		}
		break;
	case Help::RV_ERROR:
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		ShowErrorDlg((LPCTSTR)m_szErrorMsg,207);
		SendLOG( "need harddisk", 207);
		return false;
	}
	m_pAutoupDlg->SetAutoupStep( CAutoUpgradeDlg::ASP_CHK_HDD );

	SetTotalDownloadGauge(); //모두 다운로드
	
#ifdef SRC_OVERSEAS
	CheckPatchPac(); //PatchPac Download
	UpdatePatchFile();//update patch pac
#endif
	//No Update
	switch(UpdateFileLoop())
	{
	case RETURN_ERROR:
		{
		CString szMsg =STR(9);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg,206);
		SendLOG( "Fail downloading and updating", 206);
		}
		return false;
	case RETURN_STOP:
		return false;
	}

	SetFullGauge();
	SetOptimizeGauge(); //모두 다운로드
	m_pAutoupDlg->SetAutoupStep( CAutoUpgradeDlg::ASP_PATCH );
#ifdef SHIPPING
	if( (ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_SINGAPORE)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_US)		
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL))  
		SetText( STR(10)); //"패치 적용 중"

	//ioINILoaderAU kIniLoder("Save\\default\\resumefull.ini");
	//kIniLoder.SetTitle("server_patch.cfg.iop");
	//kIniLoder.SaveString("server_patch.cfg.iop", "date", m_szDate);
	DeleteGarbage(); //받은 파일은 최신파일, 기존 파일 삭제
#endif

#ifdef SHIPPING
	LOG.PrintTimeAndLog(0, "Start new pac");
	if( !ProcessNewPacFile() )
		return false;
#else
	#ifdef _DEBUG
	    LOG.PrintTimeAndLog(0, "Start new pac debug");
		if( !ProcessNewPacFile() )
			return false;
	#else // release
	    LOG.PrintTimeAndLog(0, "close pac");
		for (int i = 0; i < m_iMaxPac ; i++)
			g_PacChild(i)->CloseBigPac();	
	#endif
#endif
	SetFullGauge();

	enum { MAX_SETUP = 2, };
	char  szPaths[MAX_SETUP][MAX_PATH] = { FIREWALL_CLIENT_PATH, FIREWALL_PROTECT_PATH };
	WCHAR szNames[MAX_SETUP][MAX_PATH] = { FIREWALL_CLIENT_NAME, FIREWALL_PROTECT_NAME };
	for (int i = 0; i < MAX_SETUP-1 ; i++)  // -1: ioprotect는 미사용
	{
		char szFullPath[MAX_PATH*2];
		ZeroMemory( szFullPath, sizeof(szFullPath) );
		StringCbPrintf( szFullPath, sizeof(szFullPath), szPaths[i], (LPCTSTR)m_szRootDir );
		Help::SetUpWindowsFirewall( szFullPath, szNames[i]);
	}
#ifdef SRC_KR
	if(!ResponseServerCheck())
	{
		/*CString szMsg =STR(11);
		szMsg += m_szErrorMsg; 
		ShowErrorDlg((LPCTSTR)szMsg,11111);*/
	}
#endif
	return true;
}

//SRC_KR;WIN32;DEBUG;_WINDOWS;SHIPPING;%(PreprocessorDefinitions)

void HTTPManager::DisplayStateCopyFolder( LPVOID mySelf, bool isMaxCnt, int iCurrentCnt )
{
	HTTPManager* pHttp = (HTTPManager*) mySelf;
	if(pHttp == NULL)
		return;
	if(isMaxCnt)
		pHttp->SetMaxGauge(iCurrentCnt);
	else
		pHttp->SetCurrentGauge(iCurrentCnt);
}


void HTTPManager::_RunEnd()
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	#ifdef STEAM_ATTACH

	#endif
	if(pDlg)
	{
		switch(m_eRunEndType)
		{
		case ET_EXIT:
#ifdef STEAM_ATTACH
			{
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
#ifdef SRC_NA
				ioLocalUS * pUSLocal = (ioLocalUS * )pLocal;
				if(pUSLocal)
				{
					if(pUSLocal->IsLoginDataValidate())
						pDlg->SetWantEndDialog();
				}
#else
				ioLocalBrazil * pBRLocal = (ioLocalBrazil * )pLocal;
				if(pBRLocal)
				{
					if(pBRLocal->IsLoginDataValidate())
						pDlg->SetWantEndDialog();
				}
#endif
			}
#else
			pDlg->SetWantEndDialog();
#endif
			//pDlg->DeleteClientDirectorys( "losadown" );
			LOG.PrintTimeAndLog(0, "Complete - _RunEnd - ET_EXIT");
			break;
		case ET_RUN_LAUNCHER:
			pDlg->SetTimer(TIME_ID_LAUNCHER,1000,NULL);
			LOG.PrintTimeAndLog(0, "Complete - _RunEnd - ET_RUN_LAUNCHER");
			break;
		case ET_WAIT_CLIENT_MSG:
			pDlg->DeleteClientDirectorys( "losadown" );
			pDlg->SetTimer(TIME_ID_SECONDS_CNT,1000,NULL);
			LOG.PrintTimeAndLog(0, "Complete - _RunEnd - ET_WAIT_CLIENT_MSG");
		    break;
		case ET_ERROR_EXIT:
			pDlg->DeleteClientDirectorys( "losadown" );
			LOG.PrintTimeAndLog(0, "Complete - _RunEnd - ET_ERROR_EXIT");
			break;
		}
		m_bRunThread = false;
	}
}

void HTTPManager::_RunInit()
{
	m_bRunThread = true;
	m_bActive = true;
	m_eRunEndType = ET_EXIT;
	m_pAutoupDlg->SetAutoupStep( CAutoUpgradeDlg::APS_RUNINIT);
}

bool HTTPManager::_RunOptionCopyFolder( const CAutoUpgradeDlg * pDlg )
{
#ifdef _ADMIN_
	if(!pDlg->IsCopyFolder()) return true;
	
	if(pDlg->GetCFSourceFolder().IsEmpty()) 
	{
		SetText( STR(1) );
		LOG.PrintTimeAndLog(0, "Error -_RunOptionCopyFolder -  SourceFolder");
		return false;
	}
	if(pDlg->GetCFDestFolder().IsEmpty()) 
	{
		SetText(STR(2));
		LOG.PrintTimeAndLog(0, "Error -_RunOptionCopyFolder -  DestFolder");
		return false;
	}

	SetText( STR(3) );
	CCopyFolder cf;
	cf.AddNotCopyFileName("autoupgrade.exe");
	cf.AddNotCopyFileName("app.log");
	cf.CopyFolder(pDlg->GetCFSourceFolder(), pDlg->GetCFDestFolder(),m_bActive,DisplayStateCopyFolder, this);

	if(m_bActive)
		SetText( STR(4) );
	else
		return false;
#endif
	return true;
}

bool HTTPManager::IsCreateNewPacFile( int iCurGarbageMega )
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL) return false;
	if( pDlg->GetMaxGarbageMega() <= 0 ) 		return false;
	if(iCurGarbageMega < pDlg->GetMaxGarbageMega()) 		return false;

	return true;
}

bool HTTPManager::CreateNewPacFile(IN const int iPacArray,  IN const char * szOldPacPath, IN const char *szNewPacPath )
{
	LOG.PrintTimeAndLog(0, "New Add Start - %s.", szOldPacPath);

	if( !Help::DeleteFileRepeat( szNewPacPath) )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1),szNewPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;
	
		LOG.PrintTimeAndLog(0, "%s" , (LPCTSTR) m_szErrorMsg );
		return false;
	}

	//
	CFileException ex;
	CFile file;
	if(!file.Open(szNewPacPath,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		SetText( STR(2) );

		m_szErrorMsg = STR(3);
		m_szErrorMsg += szNewPacPath;
		m_szErrorMsg += "\r\n";
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		m_szErrorMsg += szError;
		m_szErrorMsg += "\r\n";
		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
		return false;	
	}
	const int iDataSize = 22;
	BYTE szNewPacData[iDataSize]={80,75,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // 빈 zip 파일 생성.
	file.Write((LPCSTR)szNewPacData,iDataSize);
	file.Close();    
	//

	if(!g_PacChild(OLD_PAC))
	{
		SetText(STR(4));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(5),szOldPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	char szPacPassword[MAX_PASSWORD+1]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );

	if(!g_PacChild(OLD_PAC)->OpenBigPac(szOldPacPath, true, true, szPacPassword ) )
	{
		SetText(STR(6));

		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(7), szOldPacPath, GetLastError(), PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	if(!g_PacChild(OLD_PAC)->InitSetIter())
	{
		g_PacChild(OLD_PAC)->CloseBigPac();
		SetText(STR(8));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(9),szOldPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	if(!g_PacChild(NEW_PAC))
	{
		SetText(STR(10));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(11),szNewPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	if(!g_PacChild(NEW_PAC)->OpenBigPac(szNewPacPath, false, false, szPacPassword ) )
	{
		g_PacChild(OLD_PAC)->CloseBigPac();
		SetText(STR(12));

		DWORD dwCRC = 0;
		CCrc32Static::FileCrc32Streams( szNewPacPath, dwCRC );
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(13), szNewPacPath, GetLastError(), dwCRC , PacErrorReport::GetPacExceptionCode() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
		return false;
	}

	int iLastPercent = -1;
	int iMax = g_PacChild(OLD_PAC)->GetSetSize();
	for (int i = 0; i < iMax; i++)
	{
		if(!m_bActive)
			break;

		int iPercent = (int)((i+1)*100.0f / (float)iMax);
		SetOptimizeGauge(iPercent); //set optimize progress bar
		char szTempMsg[MAX_PATH*4]="";
//		int  iMaxPac = MAP_PAC + 1; //optimize count
// #ifdef SRC_EU
// 		iMaxPac = MAX_FILE_PAC;
// #endif
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(14), iPacArray+1, m_iMaxPac, iPercent);
		if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
			SetText(szTempMsg);
		iLastPercent = iPercent;

		FileInfo kFileInfo;
		if(!g_PacChild(OLD_PAC)->GetCurFileInfo(kFileInfo))
		{
			SetText(STR(15));
	
			g_PacChild(OLD_PAC)->CloseBigPac();
			g_PacChild(NEW_PAC)->CloseBigPac();

			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(16), szOldPacPath, GetLastError(), i, PacErrorReport::GetPacExceptionCode());
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
			return false;
		}

		int iSize = g_PacChild(OLD_PAC)->GetFileSize( kFileInfo.Index );
		if( iSize != BIGPAC_ERROR_SIZE )
		{
			if(iSize > 0)
			{
				if( IsDeleteFile( kFileInfo.Index, iPacArray ) )
				{
					LOG.PrintTimeAndLog(1, "New Add Delete File - %s.", kFileInfo.FileName.c_str() );
					g_PacChild(OLD_PAC)->NextSetIter();
					continue;
				}

				BYTE *pBuf = new BYTE[iSize];
				if( !g_PacChild(OLD_PAC)->GetBuffer( kFileInfo.Index, pBuf, iSize ) )
				{
					SetText(STR(17));
					SAFEDELETEARRAY( pBuf );
					g_PacChild(OLD_PAC)->CloseBigPac();
					g_PacChild(NEW_PAC)->CloseBigPac();

					char szTempMsg[MAX_PATH*4]="";
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(18), kFileInfo.FileName.c_str(),kFileInfo.HashCode,kFileInfo.Index,kFileInfo.ZipFileSize, iSize,GetLastError(), i, PacErrorReport::GetPacExceptionCode() );
					m_szErrorMsg = szTempMsg;

					LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
					return false;
				}

				char szComment[MAX_PATH]="";
				g_PacChild(OLD_PAC)->GetFileComment( szComment, sizeof( szComment ), kFileInfo.Index );
				if(!g_PacChild(NEW_PAC)->AddBigPacData(kFileInfo.FileName.c_str(), pBuf, iSize, szComment ))
				{
					SetText(STR(19));
					SAFEDELETEARRAY( pBuf );
					g_PacChild(OLD_PAC)->CloseBigPac();
					g_PacChild(NEW_PAC)->CloseBigPac();

					char szTempMsg[MAX_PATH*4]="";
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(20), kFileInfo.FileName.c_str(), kFileInfo.HashCode, kFileInfo.Index, kFileInfo.ZipFileSize, iSize, GetLastError(), i, PacErrorReport::GetPacExceptionCode() );
					m_szErrorMsg = szTempMsg;

					LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
					return false;
				}
				SAFEDELETEARRAY( pBuf );
				LOG.PrintTimeAndLog(1, "New Add File - %s.", kFileInfo.FileName.c_str());
			}
			g_PacChild(OLD_PAC)->NextSetIter();
		}
		else
		{
			SetText(STR(21));

			g_PacChild(OLD_PAC)->CloseBigPac();
			g_PacChild(NEW_PAC)->CloseBigPac();

			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(22), kFileInfo.FileName.c_str(), kFileInfo.HashCode, kFileInfo.Index, kFileInfo.ZipFileSize, iSize, GetLastError(), i, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg);
			return false;
		}
	}

	g_PacChild(NEW_PAC)->Flush();
	g_PacChild(NEW_PAC)->CloseBigPac();
	g_PacChild(OLD_PAC)->CloseBigPac();

	if(!m_bActive)
		return false;
	if(!Help::CopyFileRepeat( szNewPacPath, szOldPacPath ))
	{
		SetText(STR(23));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(24), szOldPacPath, szNewPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s" ,(LPCTSTR)m_szErrorMsg);
		return false;
	}

	if( !Help::DeleteFileRepeat( szNewPacPath) )
	{
		SetText(STR(25));
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(26),szNewPacPath, GetLastError() );
		m_szErrorMsg = szTempMsg;

		LOG.PrintTimeAndLog(0, "%s" , (LPCTSTR) m_szErrorMsg );
		return false;
	}


	LOG.PrintTimeAndLog(0, "New Add End - %s.", szOldPacPath);
	return true;
}


bool HTTPManager::InitPac()
{
	for (int i = 0; i < MAX_PAC ; i++)
	{
		char szTemp[MAX_PATH]="";
		StringCbPrintf( szTemp, sizeof(szTemp), "%d", i);
		g_Pac.Add( szTemp );

		if( g_PacChild(i) == NULL ) 
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), i , (int)GetLastError() );
			m_szErrorMsg = szTempMsg;

			ShowErrorDlg((LPCTSTR)m_szErrorMsg,213);
			SendLOG( "Fail new memory", 213);
			return false;
		}
	}

	LOG.PrintTimeAndLog(0, "Complete init pac.");
	return true;
}

HTTPManager::ReturnState HTTPManager::SetupFull( const CString &rszDownlaodFullPath,const CString &rszClientFullPath )

{
	LOG.PrintTimeAndLog(0, "Start setup full.");
	g_PacChild(RS_PAC)->CloseBigPac();
	g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SRC_EU
	g_PacChild(MAP_EN)->CloseBigPac();
	g_PacChild(MAP_DE)->CloseBigPac();
	g_PacChild(MAP_FR)->CloseBigPac();
	g_PacChild(MAP_IT)->CloseBigPac();
	g_PacChild(MAP_PL)->CloseBigPac();
	g_PacChild(MAP_TR)->CloseBigPac();
#endif
	

	switch(UnFull(rszDownlaodFullPath))
	{
	case RETURN_ERROR:
		return RETURN_ERROR;
	case RETURN_STOP:
		return RETURN_STOP;
	}

#ifdef _ADMIN_ // 낱개 파일 업데이트는 iop 파일 압축을 푼다.
	{
		CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
		if(pDlg && pDlg->IsPieceFileUpdate())
		{
			for (int i = 0; i < m_iMaxPac ; i++)
			{
				if( !UnPac(m_szPacFullPath[i], i+1, m_iMaxPac) )
				{
					char szTempMsg[MAX_PATH*4]="";
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), i+1, (LPCTSTR) m_szErrorMsg );
					ShowErrorDlg(szTempMsg,0);
					return RETURN_ERROR; 
				}
			}
			if(!m_bActive)
				return RETURN_STOP;
		}
	}
#endif

	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg)
	{
		if(!Help::DeleteFileRepeat( (LPCTSTR) rszClientFullPath ) )
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2),(LPCTSTR) rszClientFullPath, GetLastError() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
			return RETURN_ERROR;
		}

		CFileException ex;
		CFile file; //fullip 버전 파일 생성
		if(!file.Open((LPCTSTR)rszClientFullPath,CFile::modeCreate|CFile::modeWrite,&ex)) 
		{
			m_szErrorMsg = STR(3);
			m_szErrorMsg += rszClientFullPath;
			m_szErrorMsg += "\r\n";
			TCHAR szError[1024];
			ex.GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			m_szErrorMsg += ", ";
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
			return RETURN_ERROR;	
		}
		//2013,4,10,0
		// write version.
		char szText[MAX_PATH]="";
		StringCbCopy(szText, sizeof(szText), pDlg->GetCmdFullZipVer());
		file.Write(szText, strlen(szText));
		file.Close();
		// 버전정보 작성했으니 다운받았던 /losadown/full.zip파일 삭제
		if(!file.Open((LPCTSTR)rszDownlaodFullPath,CFile::modeRead,&ex)) 
		{
			m_szErrorMsg = STR(3);
			m_szErrorMsg += rszClientFullPath;
			m_szErrorMsg += "\r\n";
			TCHAR szError[1024];
			ex.GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			m_szErrorMsg += ", ";
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
			return RETURN_ERROR;
		}
		else
		{
			file.Close();
			if(DeleteFile(rszDownlaodFullPath) != 0)
				LOG.PrintTimeAndLog(0, "%s", "write complete version info and delete download file " );
			else
				LOG.PrintTimeAndLog(0, "%s", "Can't find delete file" );
			
		}

	}

	for (int i = 0; i < m_iMaxPac ; i++)
	{
		char szPacPassword[MAX_PASSWORD+1]="";
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );
		if(!g_PacChild(i)->OpenBigPac(m_szPacFullPath[i], false, true, szPacPassword ) )
		{
			SetText(STR(4));

			DWORD dwCRC = 0;
			CCrc32Static::FileCrc32Streams(m_szPacFullPath[i], dwCRC);

			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(5), i+1 , m_szPacFullPath[i], GetLastError() ,dwCRC, PacErrorReport::GetPacExceptionCode() );
			m_szErrorMsg = szTempMsg;

			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);
			return RETURN_ERROR;
		}	
	}
#ifdef SRC_OVERSEAS
	// 바로가기 만들기 (국내 제외)
	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->CreateShortcuts( m_szRootDir, m_szShorcutName, m_szLoginURL );
	LOG.PrintTimeAndLog(0, "Complete setup full.");
#endif
	return RETURN_NEXT;
}


bool HTTPManager::ParseDeleteFileList( const char *szPath )
{
	SetText(STR(1));
	for (int i = 0; i < m_iMaxPac ; i++)
		m_vDeleteFileList[i].clear();	

	FILE *fp=NULL;
	char buffer[MAX_PATH]="";
	char szDeleteFileName[MAX_PATH]="";
	fp = fopen(szPath,"rt");
	if(fp == NULL)
	{
		// 파일이 없으면 다음 접속시에 다시 받게 되므로 정상 처리함.
		return true;
	}
	do
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		int bufferSize = strlen(buffer);
		if( bufferSize != 0)
		{
			if(buffer[bufferSize-1] == '\n')
				buffer[bufferSize-1] = 0;
		}
		
		int     len = ::strlen(buffer);
		int     iCnt = 0;
		memset(szDeleteFileName, 0, sizeof(szDeleteFileName) );

		ParseOneLine(buffer, false, true); //버전패치 사용하는 함수는 버전패치 변수가 들어감

		for(int i = 0;i < len;i++)
		{
			if(buffer[i] =='[' || buffer[i] == ']') continue;
			if(iCnt >= MAX_PATH) break;
			szDeleteFileName[iCnt++] = buffer[i];
		}
		
		if( strncmp( szDeleteFileName, "_map", 4 ) == 0 )
		{
			CString szInPath = szDeleteFileName;
			szInPath = szInPath.Right( szInPath.GetLength() - 5); // _map/ 삭제
			m_vDeleteFileList[MAP_PAC].push_back(szInPath);			
		}
		else if(  strcmp( szDeleteFileName, "" ) != 0 )
		{
			m_vDeleteFileList[RS_PAC].push_back(szDeleteFileName);
		}

	}while(!feof(fp));
	fclose(fp);

	LOG.PrintTimeAndLog(0, "Complete parse delete file.");
	return true;
}

int HTTPManager::GetDeleteFilesMegaSize()
{
	if(access(m_szPacFullPath[0],6) || access(m_szPacFullPath[1],6))
	{
		LOG.PrintTimeAndLog(0, "파일 읽기 또는 쓰기 권한이 없습니다.");
	}
	

	int iDeleteFilesSize = 0;

	for (int i = 0; i < m_iMaxPac ; i++)
	{
		if(m_vDeleteFileList[i].empty()) continue;
		if(m_vListDeleteFile[i].empty()) continue;
		if ( g_PacChild(i) == NULL )   	 continue;

		CString szPacName;
		SetPrintPacName(szPacName, i);
		
		for(size_t j = 0; j < m_vListDeleteFile[i].size(); ++j)
		{
			CString str = (LPCTSTR)m_vListDeleteFile[i][j].m_szLocalFilePath;
			CString str2 = (LPCTSTR)m_vListDeleteFile[i][j].m_szRemoteFilePath;

			if(str.GetLength() > 0)
			{
				CString Cmp =" ";

				for(int k = 0; k < str.GetLength(); ++k)
				{
					if(k > 0 || (str.GetAt(k) == Cmp) )
						break;
					else
						continue;
				}
			}

			int iCurSize = g_PacChild(i)->GetFileComprSize( (LPCTSTR)str );
			if( iCurSize != BIGPAC_ERROR_SIZE) 
			{
				iDeleteFilesSize += iCurSize;
				//deletefileinzip 구조상 crc값이 달라야 삭제함.
				g_PacChild(i)->DeleteFileInZip((LPCTSTR)str,-1);
				LOG.PrintTimeAndLog(0, "Delete file list (%s) : %s",szPacName, (LPCTSTR)str);
			}
			else if(str2.Find('/') == -1)
			{
				if(::DeleteFile(str))
					LOG.PrintTimeAndLog(0, "Delete file list NON-PAC : %s",(LPCTSTR)str);
			}
		}
		
	}

	return (iDeleteFilesSize/MEGA);
}

bool HTTPManager::IsDeleteFile( const int iIndex , int iPacArray )
{
	if( iPacArray < 0 || iPacArray >= m_iMaxPac ) return false;
	if( m_vDeleteFileList[iPacArray].empty() ) return false;
	if( g_PacChild( OLD_PAC ) == NULL ) return false;

	for(vCString::const_iterator it = m_vDeleteFileList[iPacArray].begin(); it != m_vDeleteFileList[iPacArray].end(); ++it)
	{
		if( g_PacChild(OLD_PAC)->GetFileIndex( (LPCTSTR)(*it)) == iIndex )
			return true;
	}

	return false;
}

bool HTTPManager::ProcessNewPacFile()
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( m_bWaitBeforeNewpac )
		SetTextWaitNewPac(pDlg);

	if( pDlg )
		pDlg->SetOptimizeBtnDisable();

	int iCurGarbageMega = GetAllGarbageMega();
	// close전에 garbage를 계산해야 함.
	g_PacChild(RS_PAC)->Flush();
	g_PacChild(RS_PAC)->CloseBigPac();
	g_PacChild(MAP_PAC)->Flush();
	g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SRC_EU
	g_PacChild(MAP_EN)->Flush();
	g_PacChild(MAP_EN)->CloseBigPac();
	g_PacChild(MAP_DE)->Flush();
	g_PacChild(MAP_DE)->CloseBigPac();
	g_PacChild(MAP_FR)->Flush();
	g_PacChild(MAP_FR)->CloseBigPac();
	g_PacChild(MAP_IT)->Flush();
	g_PacChild(MAP_IT)->CloseBigPac();
	g_PacChild(MAP_PL)->Flush();
	g_PacChild(MAP_PL)->CloseBigPac();
	g_PacChild(MAP_TR)->Flush();
	g_PacChild(MAP_TR)->CloseBigPac();
#endif
	
	if( !m_bActive )
		return false;

	bool bOptimizeChecked = false;
	if( pDlg && pDlg->IsOptimizeChecked() )
		bOptimizeChecked = true;
		
	bool bChangedOptimizeBtn = false;	
	if( bOptimizeChecked && !IsModifiedPacSizeAfterOptimize() )
	{
		bOptimizeChecked    = false;
		bChangedOptimizeBtn = true;
	}
#ifdef _ADMIN 
/************************************************************
admin 일 경우 server_patch.cfg파일을 (shipping/admin)2번 받는다. 그러므로
중복 파일 삭제시 문제가 발생한다. 그러므로 기존의 최적화 방식을 사용한다.
************************************************************/
	if( bOptimizeChecked && !IsManualMaxOptimizeMega( iCurGarbageMega ) ) //버튼 체크시 무조건 최적화
	{
		bOptimizeChecked    = false;
		bChangedOptimizeBtn = true;
	}
#endif
	// 모든 big pac을 읽지 않고 현재 데이터만으로 최적할지 확인
	if( !IsCreateNewPacFile(iCurGarbageMega+(m_dwAddPacSize/MEGA)) && !bOptimizeChecked )
	{
		if( bChangedOptimizeBtn )
		{
			SetText( STR(1) );
			SuspendRun( 2000 );
		}
		return true;
	}

	// 모든 big pac을 읽어서 정확히 최적할지 확인
	if( !IsCreateNewPacFile( iCurGarbageMega ) && !bOptimizeChecked )
	{
		if( !OpenAllBigPacForGarbageSize() )
			return false;

		iCurGarbageMega = GetAllGarbageMega();
		// close전에 garbage를 계산해야 함.
		g_PacChild(RS_PAC)->CloseBigPac();
		g_PacChild(MAP_PAC)->CloseBigPac();
#ifdef SRC_EU
		g_PacChild(MAP_EN)->CloseBigPac();
		g_PacChild(MAP_DE)->CloseBigPac();
		g_PacChild(MAP_FR)->CloseBigPac();
		g_PacChild(MAP_IT)->CloseBigPac();
		g_PacChild(MAP_PL)->CloseBigPac();
		g_PacChild(MAP_TR)->CloseBigPac();
#endif
		
	}

	// 최신 데이터로 최종 확인
	if( IsCreateNewPacFile( iCurGarbageMega ) || bOptimizeChecked )
	{
		if( !CreateAllNewPacFile() ) //여기서 다운로드 시작
			return false;
		SavePacSizeAfterOptimize();
	}
	else
	{
		if( bChangedOptimizeBtn )
		{
			SetText( STR(2) );
			SuspendRun( 2000 );
		}
	}

	return true;
}

bool HTTPManager::ProcessCheckPac()
{
	ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
	kIniLoder.SetTitle("Setting");
	if( !kIniLoder.LoadBool( "PacDamage", false ) ) return true;

	kIniLoder.SaveBool( "PacDamage" , false );
	
	HWND hWnd = NULL;
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg)
		hWnd = pDlg->GetSafeHwnd();

	if( Help::MessageboxShow( hWnd, STR(1),
		APP_ERROR_NAME,
		MB_YESNO | MB_TOPMOST ) == IDNO ) return true;


	for (int iArray = 0; iArray < m_iMaxPac ; iArray++)
	{
		if( !CheckPac( m_szPacFullPath[iArray], iArray+1, m_iMaxPac) )
		{
			if( !m_bActive) return false;

			Help::MessageboxShow( hWnd, STR(2),				        
				APP_ERROR_NAME,
				MB_TOPMOST );

			// full.zip 다시 받기
			CString szFullPath = m_szRootDir;
			szFullPath += "\\full.zip";
			if(!Help::DeleteFileRepeat( (LPCTSTR) szFullPath) )
			{
				char szTempMsg[MAX_PATH*4]="";
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), iArray+1, (LPCTSTR) szFullPath, GetLastError() );
				m_szErrorMsg = szTempMsg;

				ShowErrorDlg( (LPCTSTR) m_szErrorMsg ,224);
				SendLOG( "Fail deleting file in checking pac", 224);
				LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
				return false;
			}
			LOG.PrintTimeAndLog(0, "Complete - CheckPac - ReDownload full.zip - %d", iArray+1);
			return true;
		}
	}
	return true;
}

void HTTPManager::SendLOG( const char *szLog, int iErrorNumber )
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL) return;

	pDlg->SendLOG( szLog, iErrorNumber );
}

int HTTPManager::GetDeleteFileListMBWithLoad()
{
	//return 0;
	//이 함수에서 처리를할것
	int iCurDeleteFileListMega = 0;

		
	char szPath[MAX_PATH*2]="";
	StringCbPrintf(szPath, sizeof(szPath), "%s\\delete_file_list.cfg", m_szRootDir);

	//g_Pac.UnPacPieceFile(szPacPath);
	//parsedeletefilelist에서 하거나 함수 새로해서 rs와 map을 나눠서 삭제해야할 파일 경로를 푸시백
	ParseDeleteFileList(szPath);
	SetFileAttributes(szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szPath);
	iCurDeleteFileListMega = GetDeleteFilesMegaSize();


	return iCurDeleteFileListMega;
}

void HTTPManager::SuspendRun( DWORD dwStopTime )
{
	if( m_hWaitEvent )
	{
		CloseHandle( m_hWaitEvent );
		m_hWaitEvent = NULL;
	}

	m_hWaitEvent = CreateEvent(NULL, FALSE, FALSE, NULL );
	if( m_hWaitEvent )
	{
		WaitForSingleObject( m_hWaitEvent , dwStopTime );
	}
}

void HTTPManager::ResumeRun()
{
	if( m_hWaitEvent )
	{
		SetEvent( m_hWaitEvent );
		CloseHandle( m_hWaitEvent );
		m_hWaitEvent = NULL;
	}
}

bool HTTPManager::IsModifiedPacSizeAfterOptimize()
{
	ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
	kIniLoder.SetTitle("Setting");


	for (int i = 0; i < m_iMaxPac ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "PacSize%d", i+1 );
		DWORD dwINISize = kIniLoder.LoadInt( szKeyName , 0 );	
		DWORD dwCurSize = Help::GetFileSize( m_szPacFullPath[i] );
		if( dwCurSize != dwINISize )
			return true;
	}
	return false;
}

bool HTTPManager::IsManualMaxOptimizeMega( int iCurGarbageMega )
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if(pDlg == NULL) return false;

	if( pDlg->GetManualMaxOptimizeMega() <= 0 ) return false;
	if( iCurGarbageMega < pDlg->GetManualMaxOptimizeMega()) return false;

	return true;
}

void HTTPManager::SavePacSizeAfterOptimize()
{
	Help::CreateDefaultSettingINI();
	ioINILoaderAU kIniLoder("Save\\default\\setting.ini");
	kIniLoder.SetTitle("Setting");


	for (int i = 0; i < m_iMaxPac ; i++)
	{
		char szKeyName[MAX_PATH]="";
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "PacSize%d", i+1 );
		kIniLoder.SaveInt( szKeyName , Help::GetFileSize( m_szPacFullPath[i] ) );	
	}
}

int HTTPManager::GetAllGarbageMega()
{
	int iCurGarbageMega = 0;


	for (int iArray = 0; iArray < m_iMaxPac ; iArray++)
		iCurGarbageMega += (int)( g_PacChild(iArray)->GetGarbageSize()/MEGA );	
	iCurGarbageMega += GetDeleteFileListMBWithLoad();

	return iCurGarbageMega;
}

bool HTTPManager::CreateAllNewPacFile()
{
	const char szNewPacPath[MAX_FILE_PAC][MAX_PATH] = 
				{ "%s\\losadown\\rs.iop", "%s\\losadown\\map.iop", "%s\\losadown\\map_en.iop", "%s\\losadown\\map_fr.iop", "%s\\losadown\\map_de.iop", 
					"%s\\losadown\\map_it.iop", "%s\\losadown\\map_pl.iop", "%s\\losadown\\map_tr.iop" };



	for (int i = 0; i < m_iMaxPac ; i++)
	{
		// new rs.iop 생성
		char szNewPacPullPath[MAX_PATH]="";
		StringCbPrintf(szNewPacPullPath, sizeof(szNewPacPullPath), szNewPacPath[i], m_szRootDir);
		if(!CreateNewPacFile( i, m_szPacFullPath[i], szNewPacPullPath ))
		{
			if(m_bActive)
			{
				char szTempMsg[MAX_PATH*4]="";
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), i+1, (LPCTSTR) m_szErrorMsg );
				ShowErrorDlg(szTempMsg,212);
				SendLOG( "Fail optimization", 212);
			}
			return false;
		}	
	}

	return true;
}

bool HTTPManager::OpenAllBigPacForGarbageSize()
{

	for (int i = 0; i < m_iMaxPac ; i++)
	{
		char szPacPassword[MAX_PASSWORD+1]="";
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
			pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD );
		if(!g_PacChild(i)->OpenBigPac(m_szPacFullPath[i],false, true, szPacPassword ))
		{
			char szTempMsg[MAX_PATH*4]="";
			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), i+1, m_szPacFullPath[i], GetLastError());
			m_szErrorMsg = szTempMsg;

			ShowErrorDlg( (LPCTSTR) m_szErrorMsg ,225);
			SendLOG( "Fail openning BigPac", 225);
			LOG.PrintTimeAndLog(0, "%s", (LPCTSTR) m_szErrorMsg );
			return false;
		}	
	}

	return true;
}

void HTTPManager::SetTextWaitNewPac( CAutoUpgradeDlg * pDlg )
{
	if( pDlg )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), WAIT_TIME_OPTIMIZE/1000 );
		SetText( szTempMsg );
		pDlg->SetTimer( TIME_ID_WAIT_OPTIMIZE, 1000, NULL );
	}
	SuspendRun( 10000 );
}

bool HTTPManager::DecryptFile( const CString &rszFileName )
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
	if( !Help::DeleteFileRepeat( rszFileName ) )
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

void HTTPManager::CopyVersionupFiles()
{
	if( m_szCopyVersion.IsEmpty() )
		return;

	char szTemp[MAX_PATH*2]="";
	StringCbPrintf( szTemp, sizeof( szTemp ), "%s\\versionup\\%s\\", m_szRootDir, (LPCTSTR)m_szCopyVersion );
	
	CString szRoot = szTemp;
	MakeFilesList( szRoot );

	if( m_vFilePathList.empty() )
		return;

	int iLastPercent = -1;
	int iSize = m_vFilePathList.size();
	for (int i = 0; i < iSize ; i++)
	{
		CString szPath = m_vFilePathList[i];
		CString szShortPath = szPath.Right( szPath.GetLength() - ( strlen( m_szRootDir ) + strlen( "\\versionup\\YYYYMMDD\\" ) ) ); // 버전날짜는 8자 고정

		char szNewPath[MAX_PATH*2] = "";
		StringCbPrintf( szNewPath, sizeof( szNewPath ), "%s\\losadown\\%s", m_szRootDir, (LPCTSTR) szShortPath  );

		char szDir[MAX_PATH*2] = "";		
		::_splitpath((LPCTSTR)szNewPath,NULL,szDir,NULL,NULL);

		Help::SetCreateDirectoryByFullPath( szDir );
		Help::MoveFileRepeat( (LPCTSTR) szPath, (LPCTSTR) szNewPath );
		
		int iPercent = (int)((i+1)*100.0f / (float)iSize);
		char szTempMsg[MAX_PATH]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), iPercent);
		if (iPercent != iLastPercent) //UJ 130827, too much SetText() causes serious lag (+25seconds!)
			SetText(szTempMsg);
		iLastPercent = iPercent;

		if(!m_bActive) 
			return;
	}

	StringCbPrintf( szTemp, sizeof( szTemp ), "versionup\\%s", (LPCTSTR)m_szCopyVersion );
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();
	if( pDlg )
		pDlg->DeleteClientDirectorys( szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%s\\versionup\\%s\\", m_szRootDir, (LPCTSTR)m_szCopyVersion );
	RemoveDirectory( szTemp );
}

void HTTPManager::FindFileRecursive( const char *szPath )
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

void HTTPManager::MakeFilesList( const CString &rszRoot )
{
	m_vFilePathList.clear();
	if(!rszRoot.IsEmpty())
	{
		CString szFindPath = rszRoot.Left(rszRoot.GetLength() - 1); // \\ 제거
		FindFileRecursive((LPCTSTR)szFindPath);
	}
}

void HTTPManager::SetOptimizeGauge(int iPersent)
{
	CAutoUpgradeDlg *pDlg = (CAutoUpgradeDlg*) AfxGetApp()->GetMainWnd();

	if(pDlg)
	{
		pDlg->m_ProgressBar.ChangeProgressBmp();	 //progress bar bmp chage
		pDlg->m_ProgressBar.SetMaxPosition(100); //100 persent
		pDlg->m_ProgressBar.SetCurPosition( iPersent );
		pDlg->m_ProgressBar.Invalidate();
		Sleep(1); // 메인스레드가 동작할 수 있도록
	}
	return;
}
#ifdef SHIPPING
bool HTTPManager::CheckOverlapFile( const PATCHINFOFILE* pNext )
{
	int		iIndex		= 0;
	DWORD	dwCrc		= 0;
	int		iPacState	= pNext->m_eFileType - 1;

	if(IsPutPacFile((FileType)iPacState))
	{
		while(true)
		{
			iIndex = g_PacChild( iPacState )->GetFileIndex( pNext->m_szLocalFilePath );
			if( iIndex > 0 )
				dwCrc = g_PacChild( iPacState )->GetFileCRC( iIndex );
			else
				break; //못찾음

			if( pNext->m_crc == dwCrc && dwCrc > 0 )
				return false;
			else 
			{
				g_PacChild( iPacState )->RemoveBigPacInFile(iIndex);
				LOG.PrintTimeAndLog( 0, "%s Delete File pac:%d, index:%d, Name:%s ", __FUNCTION__, iPacState, iIndex, pNext->m_szLocalFilePath);
			}
		}
	}
	return true;
}

/*****************************************************************
다운 받은 리스트 중 실제 팩파일에 중복된 파일이 있는지 검사합니다.
중복된 파일이 있는 경우 삭제합니다.
*******************************************************************/
void HTTPManager::DeleteGarbage()
{
	int iOverCheckMax = 0;
	int iOverCheckCnt = 0;
	bool bPrintText = false;

	if( (ioLocalManager::GetLocalType()	   == ioLocalManager::LCT_KOREA)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_SINGAPORE)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_US)		
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND)
		|| (ioLocalManager::GetLocalType() == ioLocalManager::LCT_BRAZIL))  
	{
		bPrintText = true;
		for( int i = 0 ; i < MAX_PAC ; ++i ) //중복 파일 체크할 전체 사이즈 구함
			iOverCheckMax += m_vecOverlap[i].size();
	}

	//OverlapDelete
	for(int i = 0 ; i < MAX_PAC ; ++ i)
	{
		if( m_vecOverlap[i].empty() )
			continue;
		else
		{
			if( m_bShowGauge )
				SetMaxGauge( iOverCheckMax );
			vector<Overlapdata>::iterator Overiter;
			for(Overiter = m_vecOverlap[i].begin(); Overiter != m_vecOverlap[i].end() ; ++Overiter)
			{
				if( m_bShowGauge )
					SetCurrentGauge( iOverCheckCnt );
				g_PacChild( i )->DeleteFileInZip( Overiter->first , Overiter->second );
				++iOverCheckCnt;
				if(bPrintText)
				{
					char szTempMsg[MAX_PATH*4]="";
					int iPercent = (int)(iOverCheckCnt*100.0f / (float)iOverCheckMax);
					iPercent = max( 0, min( iPercent, 100 ) );
					SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(1), iPercent, 100);
					SetText(szTempMsg);
				}
			}
			continue;
		}
	}
	return;
}
// EDEW3940FVDP4950,10,20,30,1,autoupgrade_info.ini,0,0,1,365849202,0?4fc2974a6cb3e6c602276623c3fbfc10a9c8d463433282ee41?0?85922080363692?2013,4,10,0 대만

bool HTTPManager::CheckPatchPac()
{
	SetText( STR(1));
	//Check Local Last Patch
	GetLocalVersion( m_strLocalVersion );

	//Get Version Patch Url
	int iPoint = m_szPatchURL.Find("Client"); //http://cdn.lostsaga.com/client/ 이므로 경로 수정
	if(iPoint < 0)
		iPoint = m_szPatchURL.Find("ls-tw"); //http://cdn.lostsaga.com/client/ 이므로 경로 수정
	m_strPatchURL = m_szPatchURL.Left( iPoint );
	m_strPatchURL += "/version/";

	CString szDownloadedFullPath = m_szRootDir;
	szDownloadedFullPath +="\\losadown\\";
	szDownloadedFullPath += VERSIONPATCHINFO_FILE_NAME;
	DWORD dwDownloadedFileSize = 0;
	m_iDisplayCnt = 1;

	//download version pac, 원격지 파일 사이즈를 알 수 없으므로 0으로 설정함.
	if( !DownloadFile( m_strPatchURL + VERSIONPATCHINFO_FILE_NAME, szDownloadedFullPath, 303, dwDownloadedFileSize ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - Download file error -%s", __FUNCTION__ , VERSIONPATCHINFO_FILE_NAME);
		return false;
	}

	//check current version
	m_strCurrentPatchVersion		= "";
	m_strDownloadPossibleVersion	= "";
	m_strPrePacSize					= "";
	FILE *fp;
	char buffer[MAX_PATH];
	CString strInfo;
	strcpy(buffer, szDownloadedFullPath);
	fp = fopen(buffer,"rt");

	if(fp == NULL)
	{
		LOG.PrintTimeAndLog( 0 , "%s - Can't Found Version Info file", __FUNCTION__);
		return false;
	}

	while(true)
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		strInfo = buffer; //맨 위에 줄만 사용한다.

		if( strInfo.IsEmpty() )
			break;

		bool bSub = false;
		if( -1 != strInfo.Find("PatchVersion"))
		{
			for( int i = 0 ; i < strInfo.GetLength() ; ++i )
			{
				char ch = strInfo.GetAt(i);
				if( ch == ',' )
				{
					bSub = true; 
					continue;
				}
				if( ch >= '0' && ch <= '9' )
				{
					if(bSub)
						m_strCurrentPatchSub += ch;
					else
						m_strCurrentPatchVersion += ch;
				}
			}
		}

		if( -1 != strInfo.Find("DownloadPossibleVer"))
		{
			bool bFind = false;
			for( int i = 0 ; i < strInfo.GetLength() ; ++i )
			{
				char ch = strInfo.GetAt(i);
				if( ch >= '0' && ch <= '9' )
				{
					if( !bFind) m_strDownloadPossibleVersion += ch;
					if( bFind )
					{
						if( ch == ',') continue;
						m_strPrePacSize += ch; //pac파일 사이즈를 체크
					}
				}
				if( !bFind && ch =='?' )
				{
					bFind = true;
				}
			}
		}
		if( -1 != strInfo.Find("PatchInfo"))
		{
			OLD_PAC_INFO OldPacInfo;
			CString szTemp;
			for( int i = 0 ; i < strInfo.GetLength() ; ++i )
			{
				char ch = strInfo.GetAt(i);
				if(ch == ',')
				{
					OldPacInfo.iNum = _ttoi(szTemp);
					szTemp = "";
					continue;
				}
				if( ch >= '0' && ch <= '9' )
				{
					szTemp += ch;
				}
			}
			OldPacInfo.iSize = _ttoi(szTemp);
			m_vecOldPacInfo.push_back( OldPacInfo );
		}
		if( -1 != strInfo.Find("SkipPatch"))
		{
			CString szTemp;
			for( int i = 0 ; i < strInfo.GetLength() ; ++i )
			{
				char ch = strInfo.GetAt(i);
				if( ch >= '0' && ch <= '9' )
				{
					szTemp += ch;
				}
			}
			m_vecSkipPac.push_back( szTemp );
		}
	}
	while(!feof(fp));
	fclose(fp);
	SetFileAttributes(szDownloadedFullPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szDownloadedFullPath);

	//dif local version & server current version & server possible version
	if( (m_strCurrentPatchVersion == m_strLocalVersion) )
	{
		LOG.PrintTimeAndLog( 0, "%s last patch version", __FUNCTION__);
		return false;
	}

	CString strPatchName;
	if( !m_strDownloadPossibleVersion.IsEmpty() && (_ttoi(m_strCurrentPatchVersion) < _ttoi(m_strDownloadPossibleVersion)) )
	{
		//patch pac 파일 다운로드
		strPatchName = m_strDownloadPossibleVersion;
		strPatchName += "/v";
		strPatchName += m_strDownloadPossibleVersion;
		strPatchName += ".iop";
		szDownloadedFullPath = m_szRootDir;
		szDownloadedFullPath +="\\losadown\\v";
		szDownloadedFullPath += m_strDownloadPossibleVersion;
		szDownloadedFullPath += ".iop";
		dwDownloadedFileSize = 0;
		CString strSaveFolder = szDownloadedFullPath;
		strSaveFolder.Replace("losadown","save");

		//이전 다운로드 받은 파일 체크
		HANDLE hFile = CreateFile(strSaveFolder, GENERIC_READ, 0 , NULL, OPEN_EXISTING, 0 , NULL );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			DWORD dwFileSize =   GetFileSize(hFile, NULL);
			CloseHandle( hFile );
			if( (DWORD)_ttoi(m_strPrePacSize) == dwFileSize )
			{
				LOG.PrintTimeAndLog(0, "%s - Same File Size Remote(%s) Local(%u)", __FUNCTION__, m_strPrePacSize, dwFileSize);
				return true;
			}
		}
		else
		{
			if( IDNO == MessageBox( NULL, "The following versions of the patch file was found.\nWould you like to next version pre-download?","Notifications", MB_YESNO) )
				return false;
		}

		if( !PerDownload( m_strPatchURL + strPatchName ,strSaveFolder, 226 ,dwDownloadedFileSize , m_strPrePacSize ))
			LOG.PrintTimeAndLog(0, "%s Fail Per Version Patch File Download", __FUNCTION__ );

		return false;
	}
	return true;
}


bool HTTPManager::PerDownload( IN const CString& rszURL, IN const CString& rszFullPath, IN int iErrorCode, OUT DWORD& rszDownloadSize, IN CString& strPrePacSize )
{
	//download version pac, 원격지 파일 사이즈를 알 수 없으므로 0으로 설정함.
	if( !DownloadFile( rszURL , rszFullPath, iErrorCode, rszDownloadSize ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - Download Pre version patch pac file error -%s", __FUNCTION__ , rszURL );
		return false;
	}
	return true;
}

bool HTTPManager::DownloadFile( IN const CString& rszURL, IN const CString& rszFullPath, IN int iErrorCode, OUT DWORD& rdwDownloadSize  )
{
	CString szURL = rszURL;
#ifdef SRC_OVERSEAS
	ULONG uRand = 0;
	srand((unsigned)time(NULL));
	uRand=( ( (long) rand()<<15) | rand() ) % 1000000;
	szURL += "?";
	char szTemp[MAX_PATH] = "";
	itoa(uRand , szTemp, 10);
	szURL += szTemp;
#endif
	switch(DownLoadProcess( szURL , 0, rszFullPath, rdwDownloadSize)) //m_bShowGauge가 true면 원격지 값 전달해 줘서 게이지 출력하자!
	{
	case RETURN_ERROR:
		{
			CString szMsg = "Error Download";
			szMsg += m_szErrorMsg; 
			ShowErrorDlg((LPCTSTR)szMsg, iErrorCode);
			SendLOG( "Fail downloading version info", iErrorCode);
		}
		return false;
	case RETURN_STOP:
		return false;
	}
	return true;
}

bool HTTPManager::UpdatePatchFile()
{
	//download patch pac
	SetText(STR(1));
	CString szDownloadedFullPath, strPatchName, strDownloadVersion;
	DWORD dwDownloadedFileSize = 0;
	szDownloadedFullPath = m_szRootDir;
	szDownloadedFullPath +="\\losadown\\";
	szDownloadedFullPath += VERSIONPATCH_FILE_NAME;
	szDownloadedFullPath += ".iop";

	int iMaxPatch = _ttoi(m_strCurrentPatchVersion);
	int iLocalVersion = _ttoi( m_strLocalVersion );
	int iCnt = 1;
	for( int iCurPac = iMaxPatch  ; iCurPac > 0 ; --iCurPac, ++iCnt ) //최신버전부터 패치 진행
	{
		char szTempMsg[MAX_PATH*4]="";
		strPatchName.Format("%d",iCurPac);
		if(      iCurPac >= 10000 )		strPatchName;
		else if( iCurPac >= 1000 )		strPatchName = "0" + strPatchName;
		else if( iCurPac >= 100 )		strPatchName = "00" + strPatchName;
		else if( iCurPac >= 10 )		strPatchName = "000" + strPatchName;
		else if( iCurPac >= 1 )			strPatchName = "0000" + strPatchName;
		strDownloadVersion = strPatchName;
		strPatchName += '/';
		strPatchName += VERSIONPATCH_FILE_NAME;
		strPatchName += ".iop";

		if( m_strLocalVersion == strDownloadVersion )
		{
			LOG.PrintTimeAndLog( 0 , "Update is Already Done - v(%s)", m_strLocalVersion );
			break;
		}
		bool bSkipPacVersion = false; //특정 패치 팩의 업데이트를 제외시킨다.
		for( int iChk = 0 ; iChk < m_vecSkipPac.size() ; ++iChk )
		{
			if( m_vecSkipPac[iChk] == strDownloadVersion )
				bSkipPacVersion = true;
		}
		if( bSkipPacVersion ) continue;

		m_bShowGauge = true; //print Gauge
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(2), iCnt, iMaxPatch - iLocalVersion);
		SetText(szTempMsg);
		//download version pac, 원격지 파일 사이즈를 알 수 없으므로 0으로 설정함.
		if( !DownloadFile( m_strPatchURL + strPatchName , szDownloadedFullPath, 303, dwDownloadedFileSize ) )
		{
			LOG.PrintTimeAndLog( 0, "%s - Download version pac file error -%s", __FUNCTION__ , VERSIONPATCHINFO_FILE_NAME);
			return false;
		}

		//unpac patch pac
		DWORD dwCRC = 0;
		DWORD dwFileSize = 0;
		if(!UnPacPieceFile( szDownloadedFullPath ))
		{
			CString szMsg ="unpac error";
			ShowErrorDlg((LPCTSTR)szMsg,304);

			char szTempMsg[MAX_PATH*2]="";
			sprintf_s( szTempMsg, sizeof( szTempMsg ), "Fail unpacing Patch info - CRC :%d, Size :%d", dwCRC, dwFileSize );
			SendLOG( szTempMsg, 228 );
			return false;
		}

		if(!ParsePatchFile( true )) //version patch 정보
		{
			CString szMsg = "patch pac error";
			ShowErrorDlg((LPCTSTR)szMsg,229);
			SendLOG( "Fail reading patch pac info", 229);
			return false;
		}

		//check sub pack file
		int iSubPacCnt = 0;
		for( int iPacChk = 0 ; iPacChk < m_vecOldPacInfo.size() ; ++iPacChk )
		{
			if( _ttoi(strDownloadVersion) == m_vecOldPacInfo[iPacChk].iNum )
			{
				iSubPacCnt = m_vecOldPacInfo[iPacChk].iSize;
			}
		}

		enum { FIRST_CHECK = 0 };
		for( int iSub = FIRST_CHECK ; iSub < iSubPacCnt + 1 ; ++iSub)
		{
			CString szSubNum;
			if( iSub > FIRST_CHECK )
			{
				szSubNum.Format("%02d",iSub);
			}

			//patch pac 파일 다운로드
			strPatchName = strDownloadVersion;
			strPatchName += "/v";
			strPatchName += strDownloadVersion;
			if(!szSubNum.IsEmpty())
			{
				strPatchName += "_";
				strPatchName += szSubNum;
			}
			strPatchName += ".iop";
			szDownloadedFullPath = m_szRootDir;
			szDownloadedFullPath +="\\losadown\\v";
			szDownloadedFullPath += strDownloadVersion;
			if(!szSubNum.IsEmpty())
			{
				szDownloadedFullPath += "_";
				szDownloadedFullPath += szSubNum;
			}
			szDownloadedFullPath += ".iop";
			dwDownloadedFileSize = 0;

			bool bPreDownload = false;
			if( iMaxPatch == iCurPac ) //이전에 다운받은게 있는가? 체크
			{
				CString strFindDir = szDownloadedFullPath;
				strFindDir.Replace("losadown","save");
				HANDLE hFile = CreateFile(strFindDir, GENERIC_READ, 0 , NULL, OPEN_EXISTING, 0 , NULL );
				if( hFile != INVALID_HANDLE_VALUE )
				{
					DWORD dwFileSize =   GetFileSize(hFile, NULL);
					CloseHandle( hFile );
					if( (DWORD)_ttoi(m_strPrePacSize) == dwFileSize )
					{
						::MoveFile(strFindDir, szDownloadedFullPath);
						bPreDownload = true;
					}
				}

			}

			if( !bPreDownload ) //패치 파일 다운로드
			{
				if( !DownloadFile( m_strPatchURL + strPatchName , szDownloadedFullPath, 303, dwDownloadedFileSize ))  //패치 다운로드
				{
					LOG.PrintTimeAndLog( 0, "%s - Download version pac file error -%s", __FUNCTION__ , VERSIONPATCHINFO_FILE_NAME);
					return false;
				}
			}

			CString szDownloadPatchPath = m_szRootDir;
			szDownloadPatchPath += "\\losadown\\v";
			szDownloadPatchPath += strDownloadVersion;

			if( iSub != FIRST_CHECK ) //add sub_num -> v00001_01
			{
				szDownloadPatchPath += "_";
				szDownloadPatchPath += szSubNum;
			}
			szDownloadPatchPath += ".iop";

			char szPacPassword[MAX_PASSWORD+1]="";
			enum { PASSWORD_TYPE = 1, };

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

			if( pLocal )
				pLocal->GetPacPassword( szPacPassword, MAX_PASSWORD, PASSWORD_TYPE );
			if( !g_PacChild(PATCH_PAC)->OpenBigPac(szDownloadPatchPath, true, true, szPacPassword ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Patch Pac open fail %s", __FUNCTION__, szDownloadPatchPath ); //7번 에러
				return false;
			}

			SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(3), iCnt, iMaxPatch - iLocalVersion);
			SetText(szTempMsg);
			//파일& 패치정보 비교
			SetMaxGauge( m_vPatchPacList.size() * 2); //2번체크 함으로 x2
			int iCheckCnt = 0;
			//1회만 실행(FIRST_CHECK), 패치팩정보와 최신 패치정보를 비교하여, 값이 같을 경우, 추가 패치를 실행하지 않음.
			if( iSub == FIRST_CHECK )
			{
				for( int j = 0 ; j < m_vPatchPacList.size() ; ++j)
				{
					iCheckCnt++;
					SetCurrentGauge(iCheckCnt);
					bool bFind = false;
					VPATCHINFOFILE::iterator iter;
					for( iter = m_vFileList.begin() ; iter < m_vFileList.end() ; )
					{
						//동일한 파일이 다른 경로에 있을수도 있다..
						if( m_vPatchPacList[j].m_crc == iter->m_crc && iter->m_eFileType == m_vPatchPacList[j].m_eFileType && iter->m_szLocalFilePath == m_vPatchPacList[j].m_szLocalFilePath)
						{
#ifdef _DEBUG
							LOG.PrintTimeAndLog(0 , "%s Double Patch Delete %s, %s", __FUNCTION__, iter->m_szLocalFilePath, m_vPatchPacList[j].m_szLocalFilePath );
#endif
							iter = m_vFileList.erase(iter++);//팩 파일과 정보가 최신파일입니다. //이후 updatelist에서 삭제
							bFind = true;
							break;
						}
						else
							++iter;
					}
					if(!bFind)
					{
						m_vecSkipFileList.push_back( m_vPatchPacList[j].m_szLocalFilePath ); //최신 파일이 아니다!
					}
				}
			}


			if( iSubPacCnt == 0 ) //SubPac이 없는 경우
			{
				for( int j = 0 ; j < m_vPatchPacList.size() ; ++j)
				{
					iCheckCnt++;
					SetCurrentGauge(iCheckCnt);
					if( !(m_vPatchPacList[j].m_crc == g_PacChild(PATCH_PAC)->GetFileCRC(j)) )
					{
						DWORD dwCRC = g_PacChild(PATCH_PAC)->GetFileCRC(j);
						LOG.PrintTimeAndLog(0, "%s can't match CRC-Info(Name)%s, (InfoCRC)%lu, (PacCRC)%lu", __FUNCTION__, m_vPatchPacList[j].m_szRemoteFilePath, m_vPatchPacList[j].m_crc, dwCRC );
						continue;
					}
				}
			}
			else
			{
				int iPacSize = g_PacChild(PATCH_PAC)->GetSetSize();
				for( int j = 0 ; j < iPacSize ; ++j )
				{
					iCheckCnt++;
					SetCurrentGauge(iCheckCnt);
					CString strCmpText;
					bool bFind = false;
					char szName[MAX_PATH] =""; 
					g_PacChild(PATCH_PAC)->GetFileName(j, szName, MAX_PATH); //config\accessory\bracelet\000_bracelet_attack.ini
					strCmpText = szName;
					strCmpText.Replace("\\","/");
					if( strCmpText.Find("_map/") != -1 )
						strCmpText	= strCmpText.Right( strCmpText.GetLength() - (strCmpText.Find("_map/") + 5));
					for( int k = 0 ; k < m_vPatchPacList.size() ; ++k ) //config/accessory/bracelet/000_bracelet_attack.ini
					{
						if( m_vPatchPacList[k].m_szLocalFilePath == strCmpText && m_vPatchPacList[k].m_crc == g_PacChild(PATCH_PAC)->GetFileCRC(j))
						{
							bFind = true;
							break;
						}
					}
					if( !bFind )
					{
						DWORD dwPACCRC = g_PacChild(PATCH_PAC)->GetFileCRC(j);
						LOG.PrintTimeAndLog(0,"%s - Can Not Found Patch Information - %s:%lu", __FUNCTION__, strCmpText, dwPACCRC );
					}
				}
			}
			if( iSub == 0 )
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(4), iCnt, iMaxPatch - iLocalVersion);
			else
				SafeSprintf( szTempMsg, sizeof( szTempMsg ), STR(6), iSub , iCnt + iSub, iCurPac + iSubPacCnt - 1);
			SetText(szTempMsg); //Update Pac[8/9]

			if(!g_PacChild( PATCH_PAC )->InitSetIter())
			{
				g_PacChild(PATCH_PAC)->CloseBigPac();
				LOG.PrintTimeAndLog(0, "%s PatchFile Fail InitSetIter", __FUNCTION__ );
				return false;
			}

			int iMax = g_PacChild( PATCH_PAC )->GetSetSize();
			SetMaxGauge( iMax );
			for( int j = 0 ; j < iMax ; ++j)
			{
				SetCurrentGauge(j);
				//get Pac File
				FileInfo kFileInfo;
				if(!g_PacChild(PATCH_PAC)->GetCurFileInfo(kFileInfo))
				{
					g_PacChild(PATCH_PAC)->CloseBigPac();
					LOG.PrintTimeAndLog(0, "%s GetCurFileInfo Fail", __FUNCTION__, szDownloadPatchPath);
					return false;
				}
				//check insert pac file
				CString strFilePath = kFileInfo.FileName.c_str();
				bool bFind =false;
				strFilePath.Replace('\\','/');
				for( int k = 0 ; k < m_vecSkipFileList.size() ; ++k)
				{
					CString strDiffName = strFilePath;
					if( strDiffName.Find("_map") != -1 )
						strDiffName	= strDiffName.Right( strDiffName.GetLength() - (strDiffName.Find("_map") + 5));
					if( strcmp( strDiffName, m_vecSkipFileList[k]) == 0 )
					{
						bFind = true;
						break;
					}
				}
				if(bFind) 
				{
					g_PacChild(PATCH_PAC)->NextSetIter();
					continue;
				}

				int iPacType = ( GetPacType( strFilePath ) - 1 );
				if( iPacType == MAP_PAC || iPacType == RS_PAC )
				{
					int iSize = g_PacChild(PATCH_PAC)->GetFileSize( kFileInfo.Index );
					if( iSize != BIGPAC_ERROR_SIZE )
					{
						if(iSize > 0)
						{
							BYTE *pBuf = new BYTE[iSize];
							if( !g_PacChild(PATCH_PAC)->GetBuffer( kFileInfo.Index, pBuf, iSize ) )
							{
								MessageBox(NULL, "Fail Get Buffer Error", "ERROR",MB_OK);
								SAFEDELETEARRAY( pBuf );
								g_PacChild(PATCH_PAC)->CloseBigPac();
								g_PacChild(iPacType)->CloseBigPac();
								//error
							}

							char szComment[MAX_PATH]="";
							g_PacChild(PATCH_PAC)->GetFileComment( szComment, sizeof( szComment ), kFileInfo.Index );
							CString strInserName = kFileInfo.FileName.c_str();

							if( iPacType == MAP_PAC )
								strInserName = strInserName.Right( strInserName.GetLength() - (strInserName.Find('\\') + 1 ));

							if(!g_PacChild(iPacType)->AddBigPacData( strInserName , pBuf, iSize, szComment ))
							{
								MessageBox(NULL, "Fail Add Big Pac", "ERROR",MB_OK);
								SAFEDELETEARRAY( pBuf );
								g_PacChild(PATCH_PAC)->CloseBigPac();
								g_PacChild(iPacType)->CloseBigPac();
								return false;
							}
							SAFEDELETEARRAY( pBuf );
#ifdef _DEBUG
							LOG.PrintTimeAndLog(1, "%s New Add File - no(%d)cmt(%s) type(%d) name(%s).", __FUNCTION__, iCurPac, szComment,iPacType, strInserName);
#endif
							//overlap check
							Overlapdata Odata; //다운받은 파일 정보
							if( g_PacChild(PATCH_PAC)->GetFileCRC( kFileInfo.Index ) != 0 ) //0이면 못받는 파일 혹은 신규파일입니다.
							{
								if( iPacType == MAP_PAC )
								{
									CString strName = kFileInfo.FileName.c_str();
									strName.Replace('/','\\');
									if( strName.Find("_map") != -1 )
										Odata.first		= strName.Right( strName.GetLength() - (strName.Find("_map") + 5));
									else
										Odata.first		= kFileInfo.FileName.c_str();
								}
								else
									Odata.first		= kFileInfo.FileName.c_str();
								int iIndex		= g_PacChild(PATCH_PAC)->GetFileIndex(kFileInfo.FileName.c_str());
								Odata.second	= g_PacChild(PATCH_PAC)->GetFileCRC(iIndex);
								m_vecOverlap[iPacType].push_back(Odata);
							}
						}
					}
					else
					{
						LOG.PrintTimeAndLog( 0 , "%s File Size Error %s", __FUNCTION__, kFileInfo.FileName.c_str() );
						continue;
					}
				}
				else
				{
					LOG.PrintTimeAndLog(1, "ExtractPac File - %s.", kFileInfo.FileName );
					ExtractPacFile( kFileInfo.Index ); //그냥 푼다.
				}
				g_PacChild(PATCH_PAC)->NextSetIter();
			}
			g_PacChild(PATCH_PAC)->CloseBigPac();
		}
	}

	//패치버젼 갱신
	SetLocalVersion( m_strCurrentPatchVersion );
	return true;
}

int HTTPManager::GetPacType( IN CString& rszFilePath )
{
	if(::strcmp(rszFilePath, "full") != 0)
	{
		if( IsPutPacFileByPath( rszFilePath ) )
		{
			switch(IsMapPacFileByPath( rszFilePath ))
			{
			case FT_PAC_MAP:
				rszFilePath = rszFilePath.Right( rszFilePath.GetLength() - 5); // _map/ 삭제
				return FT_PAC_MAP;
				break;
			default:
				return FT_PAC_RS;
				break;
			}
		}
		else
		{
			if( ::strcmp(rszFilePath, "autoupgrade.exe") != 0)
				return FT_PIECE;
			else
				return FT_AUTOUPGRADE;
		}
	}
	else
	{
		return FT_FULLZIP;
	}
}


bool HTTPManager::ExtractPacFile( int iIndex, CString& rstrOutputDirName )
{
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);
	char drive[MAX_PATH] = "";
	char dir[MAX_PATH] = "";
	::_splitpath(path ,drive,dir,NULL,NULL);
	CString ExtractPath = drive;
	ExtractPath += dir;
	ExtractPath += rstrOutputDirName;

	return g_PacChild(PATCH_PAC)->Extract( iIndex, ExtractPath);
}

void HTTPManager::GetLocalVersion( OUT CString& strVersion )
{
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;
	strVersion = "00000"; //default version
	
	hSrch = FindFirstFile( LOCAL_VERSION_INFO_FILE_NAME, &wfd);
	if(hSrch == INVALID_HANDLE_VALUE) //파일이 없으면 최신 fullinstaller로 설치됨 = 최신내용
		return ;

	char szPath[MAX_PATH] = "";
	::GetCurrentDirectory(sizeof(szPath) , szPath);
	CString strFullPath = szPath;
	strFullPath += "\\";
	strFullPath += LOCAL_VERSION_INFO_FILE_NAME;

	if(g_Pac.UnPacPieceFile(strFullPath) != CPacUtil::RV_OK)
	{
		LOG.PrintTimeAndLog(0, "UnPacPieceFile Error - %s", __FUNCTION__);
		return;
	}
	
	strFullPath = szPath;
	CString strCopyPath = szPath;
	strCopyPath += "\\losadown\\version_info.ini";
	strFullPath += "\\version_info.ini";

	CopyFile( strFullPath , strCopyPath, FALSE );
	Help::DeleteFileRepeat( strFullPath );
	if(!DecryptFile(strCopyPath))
	{
		LOG.PrintTimeAndLog( 0 , "[ERROR] %s - Faile FILE Decrypt", __FUNCTION__);
		return;
	}

	CString szINIPath = "losadown\\";
	szINIPath += "version_info.ini";
	ioINILoaderAU kIniLoder( "losadown\\version_info.ini");
	kIniLoder.SetTitle("info");

	char szPatchVersion[MAX_PATH]="";
	kIniLoder.LoadString("PatchVersion","", szPatchVersion, sizeof(szPatchVersion));

	strVersion = szPatchVersion;
	return;
}

void HTTPManager::SetLocalVersion( IN CString strVersion )
{
	char szPath[MAX_PATH] = "";
	::GetCurrentDirectory(sizeof(szPath) , szPath);
	CString strFullPath = szPath;
	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;

	strFullPath += "\\losadown\\version_info.ini";
	hSrch = FindFirstFile( strFullPath, &wfd);
	if(hSrch == INVALID_HANDLE_VALUE)
	{
		std::ofstream VersionInfo;
		VersionInfo.open( "version_info.ini", std::ios_base::out | std::ios_base::app );
		VersionInfo<< "[info]\nPatchVersion=" << strVersion << "\n";
		VersionInfo.close();
		CString strMakePath = szPath;
		strMakePath += "\\version_info.ini";
		CopyFile( strMakePath, strFullPath, FALSE );
		Help::DeleteFileRepeat( (LPCTSTR)strMakePath );
	}
	else
	{
		ioINILoaderAU kIniLoder("losadown\\version_info.ini");
		kIniLoder.SetTitle("info");
		kIniLoder.SaveString("PatchVersion", strVersion );
	}
	
	strFullPath = szPath;
	strFullPath += "\\";
	strFullPath += LOCAL_VERSION_INFO_FILE_NAME;
	Help::DeleteFileRepeat( (LPCTSTR)strFullPath );

	strFullPath = szPath;
	strFullPath += "\\losadown\\version_info.ini";
	if(!EncryptFile(strFullPath))
	{
		LOG.PrintTimeAndLog( 0 , "[ERROR] %s - Faile FILE Encrypt", __FUNCTION__);
		return;
	}
	if(!g_Pac.PacPieceFile((LPCTSTR)strFullPath, "", "" ))
	{
		LOG.PrintTimeAndLog(0 , "Pac Piece File Error! -s", __FUNCTION__);
		return;
	}

	CString strCopyPath = szPath;
	strCopyPath += "\\";
	strCopyPath += LOCAL_VERSION_INFO_FILE_NAME;
	strFullPath += ".iop";
	CopyFile( strFullPath, strCopyPath, FALSE );
	return;
}

bool HTTPManager::EncryptFile( const CString &rszFileName )
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
#endif


bool HTTPManager::DownLoadOneFile( const CString &szURL ,int iRemoteFileSize, const CString &rszDownloadedFullPath , OUT DWORD &rdwDownloadedSize )
{
	CString szErrorMsg;
	if( !m_HTTP.OpenUrl(szURL, szErrorMsg) )
	{
		m_HTTP.DeleteSession();
		return false;
	}

	CInternetFile *pIF = m_HTTP.GetInternetFile(); 

	if(!pIF)
	{
		m_HTTP.DeleteSession();
		return false;
	}


	char szDrive[MAX_PATH]= "";
	char szDir[MAX_PATH*2]= "";
	char szDriveDir[MAX_PATH*2]= "";
	::_splitpath((LPCTSTR)rszDownloadedFullPath,szDrive,szDir,NULL,NULL);
	StringCbCopy( szDriveDir , sizeof( szDriveDir ), szDrive );
	StringCbCat( szDriveDir , sizeof( szDriveDir ), szDir );
	Help::SetCreateDirectory( szDriveDir , szDir );

	if(!Help::DeleteFileRepeat( (LPCTSTR) rszDownloadedFullPath ) )
	{
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return false;
	}

	CFile file;
	CFileException ex;
	if(!file.Open((LPCTSTR)rszDownloadedFullPath,CFile::modeCreate | CFile::modeWrite, &ex))
	{
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return false;	
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
		}

		file.Flush();
		file.Close();
	}
	catch( CFileException *e )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), "FileException",Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		m_szErrorMsg = "FileException";
		m_szErrorMsg += rszDownloadedFullPath;
		m_szErrorMsg += "\r\n";

		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return false;
	}
	catch( CInternetException *e )
	{
		char szTempMsg[MAX_PATH*4]="";
		SafeSprintf( szTempMsg, sizeof( szTempMsg ), "Download Fail",Help::GetAnnounceFileName(rszDownloadedFullPath) );
		SetText(szTempMsg);

		m_szErrorMsg = "Download Fail";
		m_szErrorMsg += rszDownloadedFullPath;
		m_szErrorMsg += "\r\n";

		if(e)
		{
			TCHAR szError[1024];
			e->GetErrorMessage(szError, 1024);
			m_szErrorMsg += szError;
			e->Delete();
		}

		LOG.PrintTimeAndLog(0, "%s", (LPCTSTR)m_szErrorMsg);

		file.Close();
		m_HTTP.DeleteInternetFile();
		m_HTTP.DeleteSession();
		return false;
	}
	m_HTTP.DeleteInternetFile();
	m_HTTP.DeleteSession();
	rdwDownloadedSize = write_file_size;
	return true;
}

void HTTPManager::SetPrintPacName( CString& szPacname, int iIndex )
{
	switch(iIndex)
	{
	case 0:
		szPacname = "RS_PAC";
		break;
	case 1:
		szPacname = "MAP_PAC";
		break;
	case 2:
		szPacname = "MAP_EN";
		break;
	case 3:
		szPacname = "MAP_DE";
		break;
	case 4:
		szPacname = "MAP_FR";
		break;
	case 5:
		szPacname = "MAP_IT";
		break;
	case 6:
		szPacname = "MAP_PL";
		break;
	case 7:
		szPacname = "MAP_TR";
		break;
	case 8:
		szPacname = "FULL_PAC";
		break;
	default:
		szPacname = "this file is not RS,Map File";
		break;
	}
	/*
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
		NEW_VERSION_PAC = 14
		*/

}
