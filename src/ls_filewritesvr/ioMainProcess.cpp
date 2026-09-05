
#include "stdafx.h"
#include "ioMainProcess.h"
#include "Shutdown.h"
#include "network\ioPacketQueue.h"
#include "NodeInfo\UserNodeManager.h"

#include <IPHlpApi.h>
#pragma comment( lib, "iphlpapi.lib" )

ioMainProcess *ioMainProcess::sg_Instance = NULL;
ioMainProcess::ioMainProcess()
{
	m_szINI.Clear();
	ZeroMemory(m_szLogFolder, sizeof(m_szLogFolder));

	m_szIP.Clear();
	m_iPort = 0;

	m_pClientBind = NULL;
	m_pScheduler = NULL;
	m_pLogicThread = NULL;

	m_bWantExit	= false;
	m_bReserveLogOut = false;

	m_dwCurTime = 0;
	m_NagleTime = 0;
	m_gdiToken = 0;

	bUserUpload = false;
	m_vPathInfoVec.clear();
}

ioMainProcess::~ioMainProcess()
{
}

ioMainProcess &ioMainProcess::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioMainProcess;
	return *sg_Instance;
}

void ioMainProcess::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

bool ioMainProcess::Initialize( SchedulerNode* schedulerPointer )
{
	GdiplusStartupInput gdiInput;

	GdiplusStartup(&m_gdiToken,&gdiInput,0);

	if(!CreatePool())		return false;
	if(!LoadINI())			return false;
	if(!ListenNetwork())	return false;
	if(!StartModules())		return false;
	
	//
	m_pScheduler = schedulerPointer;
 
	return true;
}

BOOL ioMainProcess::CreatePool()
{
	// 노드 관련 생성 후 네트웍 관련 생성
	g_UserNodeManager.InitMemoryPool();

	return TRUE;
}

BOOL ioMainProcess::LoadINI()
{
	ioINILoader kLoader( "FileWriteServerInfo.ini" );
	kLoader.SetTitle( "WriteFile" );

	int iMax = kLoader.LoadInt( "Max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		char szKey[MAX_PATH]="";
		char szTemp[MAX_PATH]="";
		PathInfo kInfo;
		// ext
		StringCbPrintf( szKey, sizeof( szKey ), "Ext%d", i+1 );
		kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
		kInfo.m_sExt = szTemp;
		if( kInfo.m_sExt.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Ext[%d] Error", __FUNCTION__, i+1 );
			return FALSE;
		}

		// path
		ZeroMemory( szKey, sizeof( szKey ) );
		ZeroMemory( szTemp, sizeof( szTemp ) );
		StringCbPrintf( szKey, sizeof( szKey ), "Path%d", i+1 );
		kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
		kInfo.m_sFilePath = szTemp;
		if( kInfo.m_sFilePath.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Path[%d] Error", __FUNCTION__, i+1 );
			return FALSE;
		}	


		// Temp path
		ZeroMemory( szKey, sizeof( szKey ) );
		ZeroMemory( szTemp, sizeof( szTemp ) );
		StringCbPrintf( szKey, sizeof( szKey ), "TempPath%d", i+1 );
		kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
		kInfo.m_sTempFilePath = szTemp;
		if( kInfo.m_sTempFilePath.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s TempPath[%d] Error", __FUNCTION__, i+1 );
			return FALSE;
		}


		// Error path
		ZeroMemory( szKey, sizeof( szKey ) );
		ZeroMemory( szTemp, sizeof( szTemp ) );
		StringCbPrintf( szKey, sizeof( szKey ), "ErrorPath%d", i+1 );
		kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
		kInfo.m_sErrorFilePath = szTemp;
		if( kInfo.m_sErrorFilePath.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s ErrorPath[%d] Error", __FUNCTION__, i+1 );
			return FALSE;
		}

		m_vPathInfoVec.push_back( kInfo );
		SetCreateDirectoryByFullPath( kInfo.m_sFilePath.c_str() );
		SetCreateDirectoryByFullPath( kInfo.m_sTempFilePath.c_str() );
		SetCreateDirectoryByFullPath( kInfo.m_sErrorFilePath.c_str() );
	}

	//유저가 가지고 있는 Skin Upload
	if( !SetUserSkinUploadInfo(kLoader) )
		return FALSE;

	return TRUE;
}

BOOL ioMainProcess::SetUserSkinUploadInfo(ioINILoader& kLoader)
{
	char szKey[MAX_PATH]="";
	char szTemp[MAX_PATH]="";
	
	kLoader.SetTitle( "UserUpload" );
	bUserUpload = kLoader.LoadBool( "Use", 0 );

	//Path
	ZeroMemory( szKey, sizeof( szKey ) );
	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbPrintf( szKey, sizeof( szKey ), "Path" );
	kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
	m_stUserUploadPath.m_sFilePath = szTemp;
	if( m_stUserUploadPath.m_sFilePath.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Path Error", __FUNCTION__ );
		return FALSE;
	}	

	// Temp path
	ZeroMemory( szKey, sizeof( szKey ) );
	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbPrintf( szKey, sizeof( szKey ), "TempPath" );
	kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
	m_stUserUploadPath.m_sTempFilePath = szTemp;
	if( m_stUserUploadPath.m_sTempFilePath.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s TempPath Error", __FUNCTION__ );
		return FALSE;
	}


	// Error path
	ZeroMemory( szKey, sizeof( szKey ) );
	ZeroMemory( szTemp, sizeof( szTemp ) );
	StringCbPrintf( szKey, sizeof( szKey ), "ErrorPath" );
	kLoader.LoadString( szKey, "", szTemp, MAX_PATH );
	m_stUserUploadPath.m_sErrorFilePath = szTemp;
	if( m_stUserUploadPath.m_sErrorFilePath.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s ErrorPath Error", __FUNCTION__ );
		return FALSE;
	}

	SetCreateDirectoryByFullPath( m_stUserUploadPath.m_sFilePath.c_str() );
	SetCreateDirectoryByFullPath( m_stUserUploadPath.m_sTempFilePath.c_str() );
	SetCreateDirectoryByFullPath( m_stUserUploadPath.m_sErrorFilePath.c_str() );

	return TRUE;
}

void ioMainProcess::SetCreateDirectoryByFullPath( const char *szDir )
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

BOOL ioMainProcess::ListenNetwork()
{
	ioINILoader kLoader( "FileWriteServerInfo.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szTemp[MAX_PATH]="";
	kLoader.LoadString( "IP", "", szTemp, MAX_PATH );
	m_szIP = szTemp;
	m_iPort = kLoader.LoadInt( "PORT", 9000 );
	
	g_RecvQueue.Initialize();

	if( ! g_iocp.Initialize() )  
		return FALSE;
	
	if( ! ClientBindStart() )
		return false;

	return TRUE;
}

BOOL ioMainProcess::StartModules()
{
	// logic
	Information( "Starting processor..." );
	m_pLogicThread	= new LogicThread;
	m_pLogicThread->SetProcessor(&(g_App));

	if( ! m_pLogicThread->Begin() )
	{
		Information( "failed\n" );
		return FALSE;
	}
	Information( "done\n" );
	
	FrameTimer.Start(30.0f);
	srand(timeGetTime());

	return TRUE;
}

bool ioMainProcess::ClientBindStart()
{
	if( m_bWantExit || m_bReserveLogOut ) return false;
	if( m_pClientBind ) return false;

	// Check : m_iPort 현재 포트가 사용중인지 체크함.
	PMIB_TCPTABLE pTable = NULL;
	DWORD dwSize = 0;
		
	if( ::GetTcpTable( NULL, &dwSize, TRUE ) == ERROR_INSUFFICIENT_BUFFER )
	{
		if( dwSize > 0 )
		{
			pTable = new MIB_TCPTABLE[ dwSize ];

			if( ::GetTcpTable( pTable, &dwSize, TRUE ) != NO_ERROR )
				return false;

			if( pTable != NULL )
			{
				for( DWORD idx = 0 ; idx < pTable->dwNumEntries ; ++idx )
				{
					short localPort = ntohs( (u_short)pTable->table[ idx ].dwLocalPort );

					if( localPort == m_iPort )
					{
						LOG.PrintTimeAndLog( 0, "Port Error... Already Using... %d", m_iPort );
						delete [] pTable;
						return false;
					}
				}
			}

			delete [] pTable;
		}
	}

	m_pClientBind = new ioClientBind;
	if( !m_pClientBind->Start( (char*)m_szIP.c_str(), m_iPort ) )
	{
		LOG.PrintTimeAndLog( 0, "Socket Bind Error..." );
		return false;
	}

	char strText[MAX_PATH] = "";
	sprintf_s( strText, "%s:%d", m_szIP.c_str(), m_iPort );
	SetConsoleTitle( strText );

	return true;
}

void ioMainProcess::Process(uint32& idleTime)
{
	FrameTimer.SetFrame();
	 
	int iPacketParsingSize = 0;
	iPacketParsingSize += g_RecvQueue.PacketParsing();

	ProcessTime();
	
	if(iPacketParsingSize == 0)
	{
		idleTime = 1;
	}
	else
		idleTime = 0;
}

void ioMainProcess::ProcessTime()
{
	if( TIMEGETTIME() - m_dwCurTime < 60000 )
		return;

	CheckCreateNewLog();
	m_dwCurTime = TIMEGETTIME();
}

void ioMainProcess::PrintTimeAndLog(int debuglv, LPSTR fmt )
{
	LOG.PrintTimeAndLog( debuglv, fmt );
}

void ioMainProcess::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt )
{
	LOG.DebugLog( debuglv, filename, linenum, fmt );
}

void ioMainProcess::CheckLogAllSave()
{
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	LOG.CloseLog();
	NetLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	NetLOG.CloseLog();
	CriticalLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	CriticalLOG.CloseLog();
}

void ioMainProcess::CheckCreateNewLog( bool bStart )
{
	static char szPrevTime[MAX_PATH] = "";

	SYSTEMTIME st;
	GetLocalTime(&st);
	char szCurTime[MAX_PATH] = "";
	StringCbPrintf(szCurTime, sizeof(szCurTime), "%04d%02d%02d", st.wYear, st.wMonth, st.wDay );

	if( strcmp(szCurTime, szPrevTime) != 0)
	{
		// set ip port
		ioHashString szIP;
		int          iPort = 0;

		if( bStart )
		{
			ioINILoader kLoader( "FileWriteServerInfo.ini" );
			kLoader.SetTitle( "NETWORK" );
			char szTemp[MAX_PATH]="";
			kLoader.LoadString( "IP", "", szTemp, MAX_PATH );
			szIP    = szTemp;
			iPort = kLoader.LoadInt( "PORT", 9000 );
		}
		else
		{
			szIP  = m_szIP;
			iPort = m_iPort;
		}

		memset(szPrevTime, 0, sizeof(szPrevTime));
		StringCbCopy(szPrevTime, sizeof(szPrevTime), szCurTime);

		char TimeLogName[MAX_PATH]="";
		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\BUG%s-%s-%d.log", GetLogFolder(), szCurTime, szIP.c_str(), iPort );
		{
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			LOG.CloseLog();
			LOG.OpenLog(0, TimeLogName, true);	
			LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			//LOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\NET%s-%s-%d.log", GetLogFolder(), szCurTime, szIP.c_str(), iPort);
		{
			NetLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			NetLOG.CloseLog();
			NetLOG.OpenLog(0, TimeLogName, true);
			NetLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			//NetLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\Critical%s-%s-%d.log", GetLogFolder(), szCurTime, szIP.c_str(), iPort );
		{
			CriticalLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			CriticalLOG.CloseLog();
			CriticalLOG.OpenLog(0, TimeLogName, true);	
			CriticalLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			//LOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}
	}
}

bool ioMainProcess::Startup( const char* scriptName )
{
	SetINI(scriptName);

	const char* szINI = GetINI().c_str();
	GetPrivateProfileString("Default", "Log", "MLOG", m_szLogFolder, sizeof(m_szLogFolder), szINI);

	NetLOG.PrintTimeAndLog( 0, "{DB PACKET}:[SESSION PACKET]" );

	if(!BeginSocket())
	{
		return false;
	}

	CheckCreateNewLog( true );
	return true;
}

void ioMainProcess::Exit()
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- EXIT SERVER" );

	if(m_bWantExit)
	{
		exit(1);
	}
}

void ioMainProcess::Save()
{
	static BOOL saved = FALSE;
	if( !saved )
	{
		saved = TRUE;

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- SAVE DATA" );

		CheckLogAllSave();
		
		Sleep(300);
	}
}

void ioMainProcess::Shutdown(const int type)
{
	GdiplusShutdown(m_gdiToken);

	switch(type)
	{
	case SHUTDOWN_NONE :
		break;

	case SHUTDOWN_TEST :
		break;

	case SHUTDOWN_QUICK :
		{
			if( !m_bWantExit && !m_bReserveLogOut )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- FILEWRITESERVER QUICK EXIT" );

				m_bWantExit = true;

				Save();
				Exit();
			}
		}
		break;

	case SHUTDOWN_SAFE :
		{
			if( !m_bReserveLogOut )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- FILEWRITESERVER SAFETY EXIT" );

				m_bReserveLogOut = true;
			}
		}
		break;

	case SHUTDOWN_SERVICE :
	case SHUTDOWN_EMPTYPOOL :
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- SERVICE STOP" );

			m_bWantExit = true;
			Save();
			Exit();
		}
		break;

	case SHUTDOWN_DBAGENT :
		{
			/*LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "- GAMESERVER DBAGENT CRASH" );
			g_EtcLogMgr.WriteLOG();

			Notice();

			m_bWantExit = true;
			Save();
			Exit();*/
		}
		break;

	case SHUTDOWN_CRASH :
		{
			m_bWantExit = true;
			Save();
		}
		break;

	default :
		break;
	}
}
void ioMainProcess::GetUserUploadPath( OUT ioHashString &rsPath, OUT ioHashString &rsTempPath, OUT ioHashString &rsErrorPath )
{
	if( m_stUserUploadPath.m_sFilePath.IsEmpty() || m_stUserUploadPath.m_sTempFilePath.IsEmpty() || m_stUserUploadPath.m_sErrorFilePath.IsEmpty() )
		return;

	rsPath      = m_stUserUploadPath.m_sFilePath;
	rsTempPath  = m_stUserUploadPath.m_sTempFilePath;
	rsErrorPath = m_stUserUploadPath.m_sErrorFilePath;

	return;
}

void ioMainProcess::GetAllPath( IN const ioHashString &rsFileName, OUT ioHashString &rsPath, OUT ioHashString &rsTempPath, OUT ioHashString &rsErrorPath )
{
	enum { EXT_SIZE = 3, };
	enum { JPG = 0, GEAR, COSTUM };

	if( rsFileName.IsEmpty() )
		return;
	
	char szExt[MAX_PATH]="";
	char szFileName[MAX_PATH]="";
	_splitpath_s(rsFileName.c_str(), NULL, 0, NULL, 0, szFileName, sizeof(szFileName), szExt, sizeof(szExt));

	if( strcmp( szExt, "") == 0 )
		return;

	int iMax = m_vPathInfoVec.size();
	for (int i = 0; i < iMax ; i++)
	{
		PathInfo &rkInfo = m_vPathInfoVec[i];
		if (_strnicmp(&szExt[1], rkInfo.m_sExt.c_str(), EXT_SIZE) != 0) // szExt[1]은 .을 제외
			continue;

		if( ( strrchr( szFileName, 'c') != NULL ) && GEAR == i ) //코스튬 스킨은 "_c"가 붙습니다.
			continue;

		rsPath      = rkInfo.m_sFilePath;
		rsTempPath  = rkInfo.m_sTempFilePath;
		rsErrorPath = rkInfo.m_sErrorFilePath;
		return;
	}	
}