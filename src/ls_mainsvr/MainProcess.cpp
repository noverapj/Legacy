#include "stdafx.h"

#include "ioProcessChecker.h"
#include "ServerCloseManager.h"
#include "NodeInfo\ServerNodeManager.h"
#include "NodeInfo\MgrToolNodeManager.h"
#include "NodeInfo\GuildNodeManager.h"
#include "NodeInfo\TradeNodeManager.h"
#include "NodeInfo\CampManager.h"
#include "NodeInfo\EventGoodsManager.h"
#include "NodeInfo\ExtraItemGrowthCatalystMgr.h"
#include "NodeInfo\TournamentManager.h"
#include "NodeInfo\RelativeGradeManager.h"
#include "NodeInfo\SuperGashponLimitManager.h"
#include "Scheduler\SchedulerNode.h"
#include "ioPacketChecker.h"
#include "Network\GameServer.h"
#include "Network\iocpHandler.h"
#include "Network\ioPacketQueue.h"
#include "DataBase\DBClient.h"
#include "DataBase\LogDBClient.h"
#include "Shutdown.h"
#include "MainProcess.h"
#include "EtcHelpFunc.h"
#include "./NodeInfo/NodeInfoManager.h"
#include "../ioINILoader/ioINILoader.h"
#include "NodeInfo/SpecialShopManager.h"
#include "NodeInfo/GuildRoomInfos.h"
#include <strsafe.h>
#include "MainProcess.h"
#include "NodeInfo\MatchNodeManager.h"
#include "NodeInfo\PracticeNodeManager.h"

extern CLog TradeLOG;
extern CLog MonitorLOG;
extern CLog ProcessLOG;
extern CLog OperatorLOG;
extern CLog WemadeLOG;

bool tokenize(const std::string& str, const std::string& delimiters, std::vector<std::string>& tokens)
{
	tokens.clear();

	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	std::string token;
	while (string::npos != pos || string::npos != lastPos)
	{
		token = str.substr(lastPos, pos - lastPos);
		tokens.push_back(token);

		lastPos = str.find_first_not_of(delimiters, pos);

		pos = str.find_first_of(delimiters, lastPos);
	}
	return (tokens.size() > 0) ? true : false;
}

ioMainProcess *ioMainProcess::sg_Instance = NULL;
ioMainProcess::ioMainProcess() 
{
	m_ServerBind	= NULL;
	m_MgrToolBind	= NULL;
	m_logicThread	= NULL;
	m_bWantExit		= false;

	m_dwCurTime		= 0;

	m_iPort = 0;
	m_iTotalRegUser = 0;
	
	m_bGameServerDBAgentExtend = false;
	m_bGameServerOption = false;
	m_bExtraItemGrowthCatalyst = false;
	m_bEventShopReload = false;

	m_bUseClientVersion = false;
	m_iClientVersion    = 0;
	
	m_bTestZone = false;

	m_pExtraItemGrowthCatalystMgr = NULL;
	m_bUseManageToolPrivateIP     = false;
	m_pTournamentManager          = NULL;
	m_MatchingTime			= 0;

	ZeroMemory(m_szLogFolder, sizeof(m_szLogFolder));
}

ioMainProcess::~ioMainProcess()
{
	EndSocket();
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

bool ioMainProcess::Initialize()
{	
	if(!CreateLog())		return false;
	if(!CreatePool())		return false;
	if(!LoadINI())			return false;
	if(!ListenNetwork())	return false;
	if(!StartModules())		return false;
		
	SetBeforeLoop();
	return true;
}

bool ioMainProcess::CreateLog()
{
	const char* szINI = g_MainProc.GetINI().c_str();
	GetPrivateProfileString("Default", "Log", "MLOG", m_szLogFolder, sizeof(m_szLogFolder), szINI);

	g_MainProc.CheckCreateNewLog( true );
	return true;
}

bool ioMainProcess::CreatePool()
{
	// 노드 관련 생성 후 네트웍 관련 생성
	g_ServerNodeManager.InitMemoryPool();
	g_MgrTool.InitMemoryPool();
	g_MatchNodeManager.InitMemoryPool();
	g_GuildNodeManager.InitNodeINI();
	g_PacketChecker.LoadINI();
	g_ProcessChecker.LoadINI();
	g_RelativeGradeMgr.LoadINIData();

	if(!BeginSocket())
	{
		LOG.PrintTimeAndLog(0,"WSAStartup FAILED:%d",GetLastError());
		return false;
	}

	return true;
}

bool ioMainProcess::LoadINI()
{
	//---------------------------------
	//        ini 정보
	//---------------------------------
	const char* szINI = g_MainProc.GetINI().c_str();
	m_iPort = GetPrivateProfileInt("Default", "Port", 9000, szINI);	

	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "NETWORK" );

	char szPrivateIP[16];
	char szPublicIP[16];

	kLoader.LoadString("PrivateIP", "", szPrivateIP, sizeof(szPrivateIP));
	m_szPrivateIP = szPrivateIP;
	kLoader.LoadString("PublicIP", "", szPublicIP, sizeof(szPublicIP));
	m_szPublicIP = szPublicIP;

	if (m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty())
	{
		return false;
	}

	CheckTestZone( m_szPublicIP.c_str() );

	// Nagle
	kLoader.SetTitle( "Nagle" );
	m_nagleTime = kLoader.LoadInt( "Nagle_Time", 30 );

	g_CampMgr.LoadINIData();
	g_EventGoodsMgr.LoadINIData(true);
	g_ServerCloseMgr.LoadCloseServerInfo();

	g_PracticeNodeManager.LoadINIData();

	m_pExtraItemGrowthCatalystMgr = new ExtraItemGrowthCatalystMgr;
	m_pExtraItemGrowthCatalystMgr->LoadINIData( true );

	m_pTournamentManager = new TournamentManager;
	m_pTournamentManager->Initialize();

	m_pSuperGashponLimitManager = new SuperGashponLimitManager;
	m_pSuperGashponLimitManager->InitLimit();

	g_SpecialShopManager.LoadINI();

	m_pGuildRoomManager	= new GuildRoomInfos;

	LoadClientVersion();

	kLoader.SetTitle( "MatchingTime" );
	m_MatchingTime = kLoader.LoadInt( "Matching_time", 3 );

	/************************************************************************/
	/* Load DbagentInfo 
	   Load LOGDBAgentInfo
	   Load BillingInfo
	   */
	/************************************************************************/
	LOG.PrintTimeAndLog( 0, "%s MatchingTime %d", __FUNCTION__, m_MatchingTime );

	return g_NodeInfoManager.Verify();
}

bool ioMainProcess::SetLocalIP( int iPrivateIPFirstByte )
{
	ioHashStringVec vIPList;
	if( !Help::GetLocalIpAddressList( vIPList ) ) 
		return false;

	int iSize = vIPList.size();
#ifdef LOCAL_DBG
	if( iSize >= 1 )
	{
		m_szPublicIP  = vIPList[0];
		m_szPrivateIP = vIPList[0];
		return true;
	}
	// 1, 2 아니면 에러
	if( !COMPARE( iSize, 1, 4 ) )
#else
	if( !COMPARE( iSize, 1, 3 ) )
#endif
	
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error %d", __FUNCTION__, iSize );
		return false;
	}

	// 1
	if( iSize == 1 ) 
	{
		m_szPublicIP  = vIPList[0];
		m_szPrivateIP = vIPList[0];

		if( m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Local IP Error %s:%s", __FUNCTION__, m_szPrivateIP.c_str(), m_szPublicIP.c_str() );
			return false;
		}

		return true;
	}

	// 2
	for (int i = 0; i < iSize ; i++)
	{
		if( atoi( vIPList[i].c_str() ) != iPrivateIPFirstByte )
		{
			m_szPublicIP = vIPList[i];
		}
		else
		{
			m_szPrivateIP = vIPList[i];
		}
	}

	if( m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Local IP Empty %s:%s", __FUNCTION__, m_szPrivateIP.c_str(), m_szPublicIP.c_str() );
		return false;
	}

	return true;
}

void ioMainProcess::CheckTestZone( const char *szIP )
{
	if( strcmp( szIP, "211.239.156.226")  == 0 ||
		strcmp( szIP, "211.239.156.227")  == 0 )
	{
		m_bTestZone = true;
	}
	else
		m_bTestZone = false;
}

bool ioMainProcess::ListenNetwork()
{
	g_RecvQueue.Initialize();
	
	if(!g_iocp.Initialize()) 			return false;
	if(!g_NodeInfoManager.Load())		return false;
	if(!g_NodeInfoManager.ConnectTo())	return false;
	return true;
}

bool ioMainProcess::StartModules()
{
	// logic
	Information( "Starting processor..." );
	m_logicThread = new LogicThread;
	m_logicThread->SetProcessor(&(g_MainProc));
	
	if(!m_logicThread->Begin())
	{
		Information( "failed\n" );
		return false;
	}
	Information( "done\n" );


	// server bind
	Information( "Starting ServerBind..." );
	if( IsWantExit() ) return false;
	if( m_ServerBind ) return false;

	m_ServerBind = new ioServerBind;
	if( !m_ServerBind->Start( (char*)m_szPrivateIP.c_str(), m_iPort ) )
		return false;

	char strText[MAX_PATH] = "";
	sprintf_s( strText, "%s:%s:%d", m_szPublicIP.c_str(), m_szPrivateIP.c_str(), m_iPort );
	SetConsoleTitle( strText );
	Information( "done\n" );
	
	// mgrtool bind
	Information( "Starting MgrToolBind..." );
	if( IsWantExit() ) return false;
	if( m_MgrToolBind ) return false;

	const char* szINI = g_MainProc.GetINI().c_str();
	int iPort = GetPrivateProfileInt("Default", "ManagerToolPort", 9000, szINI);	

	ioINILoader kLoader( "ls_config_main.ini" );

	// Write 스레드 생성.
	if( !kLoader.Startup(true) )
	{
		LOG.PrintTimeAndLog( 0, "Error : Write Thread No StartUp" );
		return false;
	}

	m_bUseManageToolPrivateIP = kLoader.LoadBool( "NETWORK", "UseManageToolPrivateIP", false );



	ioHashString szManageToolIP;
	if( m_bUseManageToolPrivateIP )
		szManageToolIP = m_szPrivateIP;
	else
		szManageToolIP = m_szPublicIP;

	m_MgrToolBind = new ioMgrToolBind;
	if( !m_MgrToolBind->Start( (char*)szManageToolIP.c_str(), iPort ) )
		return false;

	Information( "done\n" );

	return true;
}

void ioMainProcess::SetBeforeLoop()
{
	g_DBClient.OnSelectTotalRegUser();
	g_DBClient.OnSelectCampData();
	g_DBClient.OnSelectCampSpecialUserCount( CAMP_BLUE );
	g_DBClient.OnSelectCampSpecialUserCount( CAMP_RED );
	g_DBClient.OnSelectGuildInfoList( GUILD_LOAD_START_INDEX, GUILD_MAX_LOAD_LIST );
	g_DBClient.OnSelectTradeItemInfo( TRADE_START_LAST_INDEX, TRADE_MAX_LOAD_LIST );
	g_DBClient.OnSelectTournamentData( TOURNAMENT_LOAD_START_INDEX, TOURNAMENT_MAX_LOAD_LIST );
	g_EventGoodsMgr.InitUserBuyCount();
	g_DBClient.OnSelectGoodsBuyCount( g_EventGoodsMgr.GetPagingSize(), g_EventGoodsMgr.GetPage() );

	g_DBClient.OnSelectPracticeInfoList(PRACTICE_LOAD_START_INDEX, PRACTICE_LOAD_START_INDEX, PRACTICE_MAX_LOAD_LIST);
}

void ioMainProcess::LoadClientVersion()
{
	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "ClientVersion" );

	m_bUseClientVersion = kLoader.LoadBool( "IsUse", false );
	m_iClientVersion    = kLoader.LoadInt( "ClientVersion", 0);

	LOG.PrintTimeAndLog( 0, "Client Version : %d:%d", (int)m_bUseClientVersion, m_iClientVersion);
}

void ioMainProcess::SaveClientVersion( bool bUseClientVerions , int iClientVersion )
{
	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "ClientVersion" );
	kLoader.SaveBool( "IsUse", bUseClientVerions );
	kLoader.SaveInt( "ClientVersion", iClientVersion );

	m_bUseClientVersion = bUseClientVerions;
	m_iClientVersion    = iClientVersion;

	LOG.PrintTimeAndLog(0, "Change Client Version : %d:%d", (int)m_bUseClientVersion, m_iClientVersion );
}

bool ioMainProcess::IsUseClientVersion() const
{
	return m_bUseClientVersion;
}

int ioMainProcess::GetClientVersion() const
{
	return m_iClientVersion;
}

void ioMainProcess::Exit()
{
	LOG.PrintTimeAndLog( 0, "- EXIT SERVER" );
	if(IsWantExit())
	{
		exit(1);
	}
}

void ioMainProcess::Save()
{
	if( !IsWantExit() )
	{
		SetExit();

		LOG.PrintTimeAndLog( 0, "- SAVE DATA" );
		
		CheckLogAllSave();

		g_EventGoodsMgr.SaveDataAllWrite();
		g_CampMgr.ProcessServerClose();
		g_GuildNodeManager.ServerDownAllUpdateGuild();
		g_DBClient.OnHeroExpertMinusToZero();
		g_DBClient.OnSelectTotalRegUser( true );
		g_TournamentManager.ServerDownAllSave();
		g_SuperGashponLimitManager.ServerDownAllSave();

		Sleep(1000);
	}
}

void ioMainProcess::Shutdown(const int type)
{
	switch(type)
	{
	case SHUTDOWN_NONE :
		break;

	case SHUTDOWN_TEST :
		//::MessageBox( GetHwnd(), "shutdown", "Lostsaga", MB_OK );
		break;

	case SHUTDOWN_QUICK :
		{
			if( !IsWantExit() )
			{
				LOG.PrintTimeAndLog( 0, "- MAINSERVER EXIT" );
				
				Save();
				Exit();
			}
		}
		break;

	case SHUTDOWN_SERVICE :
	case SHUTDOWN_EMPTYPOOL :
		{
			LOG.PrintTimeAndLog( 0, "- SERVICE STOP" );

			Save();
			Exit();
		}
		break;

	case SHUTDOWN_CRASH :
		{
			Save();
		}
		break;

	default :
		break;
	}
}

void ioMainProcess::AllServerExit( DWORD dwExitType, bool bPopup )
{
	DWORD exitType = 0;

	switch( dwExitType )
	{
	case ALL_SERVER_QUICK_EXIT:
		exitType = MSTPK_GAMESERVER_QUICK_EXIT;
		break;
	case ALL_SERVER_SAFETY_EXIT:
		exitType = MSTPK_GAMESERVER_SAFETY_EXIT;
		break;
	}

	SP2Packet kPacket( exitType );
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}

void ioMainProcess::Process(uint32& idleTime)
{
	g_ProcessChecker.MainThreadCheckTimeStart();

	FrameTimer.SetFrame();
	
	g_ProcessChecker.CallMainThread();

	// 모든 패킷의 처리
	g_PacketChecker.CheckCollectFreezing();
	int iPacketParsingSize = g_RecvQueue.PacketParsing();
	if(iPacketParsingSize == 0)
		idleTime = 1;
	else
		idleTime = 0;
	g_ProcessChecker.MainProcessMaxPacket( iPacketParsingSize );
	
	// 프로세스.
	g_ServerNodeManager.ProcessServerNode();
	g_DBClient.ProcessTime();
	g_LogDBClient.ProcessTime();
	g_MgrTool.ProcessMgrToolNode();
	g_GuildNodeManager.ProcessGuildNode();
	g_CampMgr.ProcessCamp();
	g_EventGoodsMgr.Process();
	g_TournamentManager.Process();
	g_ServerCloseMgr.ProcessCloseCheck();
	g_ProcessChecker.Process();
	g_RelativeGradeMgr.Process();
	//g_TradeNodeManager.ProcessUpdateTrade();

	g_PracticeNodeManager.ProcessPractice();

	ProcessTime();
	GameServerDBAgentExtend();
	GameServerOption();
	ExtraItemGrowthCatalyst();
	EventShopReload();

	g_ProcessChecker.MainThreadCheckTimeEnd();

	 
}

void ioMainProcess::DrawModule( MAINSERVERINFO& info )
{
	//GLOBAL TIME
	info.dwGlobalTime	= TIMEGETTIME();

	//Network Info
	strcpy_s( info.szPublicIP, sizeof(info.szPublicIP), m_szPublicIP.c_str() );
	strcpy_s( info.szPrivateIP, sizeof(info.szPrivateIP), m_szPrivateIP.c_str() );
	info.iPort			= m_iPort;

	//Thread Info
	info.ThreadCount	= ThreadManager::GetInstance()->GetHandleCount();

	//Connect Client Info
	info.JoinServerCount	= g_ServerNodeManager.GetNodeSize();

	//Remainder MemPool Info
	info.RemainderMemPoolCount	= g_ServerNodeManager.RemainderNode();

	//RECV QUEUE
	int usingCount[4], remainCount[4];
	g_RecvQueue.GetPoolCount( usingCount, remainCount );

	info.RecvQueuePacketCount[ 0 ]	= usingCount[0];
	info.RecvQueuePacketCount[ 1 ]	= usingCount[1];
	info.RecvQueuePacketCount[ 2 ]	= usingCount[2];
	info.RecvQueuePacketCount[ 3 ]	= usingCount[3];

	info.RecvQueueRemainderCount[0]	= remainCount[0];
	info.RecvQueueRemainderCount[1]	= remainCount[1];
	info.RecvQueueRemainderCount[2]	= remainCount[2];
	info.RecvQueueRemainderCount[3]	= remainCount[3];

	//DB AGENT SERVER INFO
	strcpy_s( info.szDBAgentIP, sizeof(info.szDBAgentIP), g_DBClient.GetDBAgentIP().c_str() );
	info.DBAgentPort				= g_DBClient.GetDBAgentPort();
	info.bDBAConnected				= g_DBClient.IsActive();
	strcpy_s( info.szLogDBAgentIP, sizeof(info.szLogDBAgentIP), g_LogDBClient.GetDBAgentIP().c_str() );
	info.LogDBAgentPort				= g_LogDBClient.GetDBAgentPort();
	info.bLogDBAConnected			= g_LogDBClient.IsActive();

	//GUILD INFO
	info.MaxGuildCount				= g_GuildNodeManager.GetNodeSize();
	info.MaxUpdateGuild				= g_GuildNodeManager.GetUpdateNodeSize();

	//CAMP INFO
	strcpy_s( info.szCampStringHelp, sizeof(info.szCampStringHelp), g_CampMgr.GetCampStringHelp() );
	
	//Trade Info
	info.MaxTradeItemCount			= g_TradeNodeManager.GetNodeSize();

	//Event Shop Info
	strcpy_s( info.szEventShopState, sizeof(info.szEventShopState), g_EventGoodsMgr.GetEventShopState().c_str() );
	info.EventGoodsSaveDataCount	= g_EventGoodsMgr.GetSaveDataCount();

	//MANAGER TOOL
	info.MaxToolConnectCount		= g_MgrTool.GetNodeSize();

	// Client Version
	info.bUseClientVersion			= m_bUseClientVersion;
	info.iClientVersion				= m_iClientVersion;

	// Main Server Version
	strcpy_s( info.szMainServerVersion, sizeof(info.szMainServerVersion), STRFILEVER );
	strcpy_s( info.szMainServerName, sizeof(info.szMainServerName), STRINTERNALNAME );

	// log
	LOG.GetMemoryInfo(info.remainLogCount, info.usingLogCount, info.maxUsingLogCount, info.dropLogCount);
}

void ioMainProcess::ProcessTime()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( TIMEGETTIME() - m_dwCurTime < 60000 ) return;

	CheckCreateNewLog();

	m_dwCurTime = TIMEGETTIME();
}

void ioMainProcess::CheckLogAllSave()
{
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	LOG.CloseLog();
	TradeLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	TradeLOG.CloseLog();
	MonitorLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	MonitorLOG.CloseLog();
	ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	ProcessLOG.CloseLog();
	OperatorLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	OperatorLOG.CloseLog();
	WemadeLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	WemadeLOG.CloseLog();

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
		if( !bStart )
		{
			g_PacketChecker.WriteLOG();
		}

		memset(szPrevTime, 0, sizeof(szPrevTime));
		StringCbCopy(szPrevTime, sizeof(szPrevTime), szCurTime);

		char TimeLogName[MAX_PATH]="";
		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\BUG%s.log", GetLogFolder(), szCurTime);
		{
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			LOG.CloseLog();
			LOG.OpenLog(0, TimeLogName, true);
			LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			LOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}		

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\Trade%s.log", GetLogFolder(), szCurTime);
		{
			TradeLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			TradeLOG.CloseLog();
			TradeLOG.OpenLog(0, TimeLogName, true);
			TradeLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			TradeLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\Monitor%s.log", GetLogFolder(), szCurTime);
		{
			MonitorLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			MonitorLOG.CloseLog();
			MonitorLOG.OpenLog(0, TimeLogName, true);
			MonitorLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			MonitorLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\Process%s.log", GetLogFolder(), szCurTime);
		{
			ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			ProcessLOG.CloseLog();
			ProcessLOG.OpenLog(0, TimeLogName, true);
			ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			ProcessLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\Operator%s.log", GetLogFolder(), szCurTime);
		{
			OperatorLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			OperatorLOG.CloseLog();
			OperatorLOG.OpenLog(0, TimeLogName, true);
			OperatorLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			OperatorLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "%s\\LOG%s.log", GetLogFolder(), szCurTime);
		{
			WemadeLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			WemadeLOG.CloseLog();
			WemadeLOG.OpenLog(0, TimeLogName, true);
			WemadeLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			WemadeLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}
	}
}

void ioMainProcess::GameServerDBAgentExtend()
{
	if( m_bGameServerDBAgentExtend )
	{
		// 모든 게임 서버에 DBAgent 확장 신호 전송
		SP2Packet kPacket( MSTPK_DBAGENT_EXTEND );
		g_ServerNodeManager.SendMessageAllNode( kPacket );		
		m_bGameServerDBAgentExtend = false;
		LOG.PrintTimeAndLog( 0, "GameServerDBAgentExtend All Send" );
	}
}

void ioMainProcess::GameServerOption()
{
	if( m_bGameServerOption )
	{
		m_bGameServerOption = false;
	
		SP2Packet kPacket( MSTPK_GAME_SERVER_OPTION );
		FillGameServerOption( kPacket );
		g_ServerNodeManager.SendMessageAllNode( kPacket );				
	}
}

void ioMainProcess::FillGameServerOption( SP2Packet &rkPacket )
{
	ioINILoader kLoader( "config/game_server_option.ini" );
	kLoader.SetTitle( "common" );

	bool bOnlyServerRelay       = kLoader.LoadBool( "only_server_relay", false );
	bool bNagleAlgorithm		= kLoader.LoadBool( "nagle_algorithm", false );
	bool bPlazaNagleAlgorithm   = kLoader.LoadBool( "plaza_nagle_algorithm", false );
	bool bCharChangeToUDP       = kLoader.LoadBool( "char_change_udp", false );
	bool bWholeChatOn           = kLoader.LoadBool( "whole_chat_on", true );

	rkPacket << bOnlyServerRelay << bNagleAlgorithm << bPlazaNagleAlgorithm << bCharChangeToUDP << bWholeChatOn;
	LOG.PrintTimeAndLog( 0, "FillGameServerOption : %d - %d - %d - %d - %d", (int)bOnlyServerRelay, (int)bNagleAlgorithm, (int)bPlazaNagleAlgorithm, (int)bCharChangeToUDP, (int)bWholeChatOn );
}

void ioMainProcess::ExtraItemGrowthCatalyst()
{
	if( m_bExtraItemGrowthCatalyst )
	{
		m_bExtraItemGrowthCatalyst = false;

		if( m_pExtraItemGrowthCatalystMgr )
		{
			m_pExtraItemGrowthCatalystMgr->LoadINIData( false );
			m_pExtraItemGrowthCatalystMgr->SendLoadData( NULL );
		}
	}
}

void ioMainProcess::EventShopReload()
{
	if( m_bEventShopReload )
	{
		m_bEventShopReload = false;

		// 실시간 블럭 ( INI 저장 방식이 바뀌어서 안됨 )
		// g_EventGoodsMgr.LoadINIData( false );
	}
}

void ioMainProcess::PrintTimeAndLog(int debuglv, LPSTR fmt )
{
	LOG.PrintTimeAndLog( debuglv, fmt );
}

void ioMainProcess::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt )
{
	LOG.DebugLog( debuglv, filename, linenum, fmt );
}
