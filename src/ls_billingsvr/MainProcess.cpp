// Window.cpp: implementation of the ioMainProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainProcess.h"

#include <objbase.h>
#include <strsafe.h>

#include "NodeInfo/ServerNodeManager.h"
#include "NodeInfo/GoodsManager.h"
#include "Network/GameServer.h"
#include "Network/iocpHandler.h"
#include "Network/ioPacketQueue.h"
#include "MgameBillingServer/MgameBillingServer.h"
#include "channeling/iochannelingnodemanager.h"
#include "database/logdbclient.h"
#include "Local/ioLocalManager.h"
#include "Version.h"
#include "Local/ioLocalParent.h"
#include "ioProcessChecker.h"
#include "ThailandOTPServer/ThailandOTPNodeManager.h"
#include "WemadeBillingServer/WemadeBuyServer.h"
#include "MonitoringServerNode/monitoringnodemanager.h"
#include "ToonilandBillingServer/ToonilandBillingServer.h"
#include "ThreadPool/ioThreadPool.h"
#include "ThreadPool/NexonThreadPool.h"
#include "ThailandLoginServer/ThailandLoginServer.h"
#include "ThailandIPBonusServer/ThailandIPBonusServer.h"
#include "ThailandBillingServer/ThailandBillingGetServer.h"
#include "ThailandBillingServer/ThailandBillingSetServer.h"
#include "ThailandIPBonusServer/ThailandIPBonusOutServer.h"
#include "EtcHelpFunc.h"
#include "USBillingServer/USBillingServer.h"
#include "Operation/Manager.h"
#include "Network/ConnectAssist.h"
#include "TestThread.h"
#include "NodeInfo/MgrToolNodeManager.h"
#include "NexonNISMSServer/NexonNISMSServer.h"
#include "NexonEUSessionServer/NexonEUSessionServer.h"
#include "../../extra/LS_RestAPI/ioRestAPI.h"

//#Include "Minidump/MiniDump.h"

extern CLog LOG;
extern CLog ProcessLOG;
//////////////////////////////////////////////////////////////////////////
ioMainProcess *ioMainProcess::sg_Instance = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ioMainProcess::ioMainProcess()
{
	InitData();
}

ioMainProcess::~ioMainProcess() //kyg 로그 닫는거 만들어주면 댐 
{
	//Destroy();
}

void ioMainProcess::InitData()
{
	m_pServerBind = NULL;
	m_pMonitoringBind  = NULL;
	m_pGameLogicThread = NULL;
	m_bWantExit = false;

	m_dwCurTime = 0;

	m_bReserveLogOut = false;
	m_dwReserveLogOutTime = 0;

	m_iPort = 0;
	m_iMSPort = 0;
	m_iMgrPort = 0;

	m_bInfoDraw = false;

	m_pGoodsMgr      = NULL;
	m_pChannelingMgr = NULL;
	m_pLocalMgr      = NULL;

	//	m_bTestMode      = false;
	ZeroMemory(m_szLogFolder, sizeof(m_szLogFolder));
	m_dwMainSleepMilliseconds = 0;
	m_bUseMonitoring = false;
	m_dwLogOutWaitTime = 0;
	m_cashProcessCount = 0;
	m_MgrToolBind = NULL;
}

ioMainProcess &ioMainProcess::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new ioMainProcess;
	return *sg_Instance;
}

void ioMainProcess::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

void ioMainProcess::Destroy()
{
	LOG.PrintTimeAndLog( 0, "App Close 1" );

	SAFEDELETE( m_pServerBind );         //서버를 종료하므로 더이상의 Accept를 받지 않는다.
	SAFEDELETE( m_pMonitoringBind ) ;
	SAFEDELETE( m_pGameLogicThread );
	SAFEDELETE( m_pGoodsMgr );       
	SAFEDELETE( m_pChannelingMgr );
	SAFEDELETE( m_pLocalMgr );

	LOG.PrintTimeAndLog( 0, "App Close 2" );

	//iocpHandler::ReleaseInstance();    //유저로 부터 더이상의 패킷을 받지 않는다.
	LOG.PrintTimeAndLog( 0, "App Close 3" );

	//g_ServerNodeManager.ReleaseMemoryPool();
	LOG.PrintTimeAndLog( 0, "App Close 4" );

	//	g_MonitoringNodeManager.ReleaseMemoryPool();
	LOG.PrintTimeAndLog( 0, "App Close 5" );

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{


	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		g_ThailandLoginServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-1" );

		g_ThailandOTPNodeManager.ReleaseMemoryPool();
		LOG.PrintTimeAndLog( 0, "App Close 6-2" );

		g_ThailandIPBonusServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-3" );

		g_ThailandIPBonusOutServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-4" );

		g_ThailandBillingGetServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-5" );

		g_ThailandBillingSetServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-6" );

		ThailandLoginServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-7" );

		ThailandOTPNodeManager::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-8" );

		ThailandIPBonusServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-9" );

		ThailandIPBonusOutServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-10" );

		ThailandBillingGetServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-11" );

		ThailandBillingSetServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-12" );
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
	{
		g_USBillingServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-1" );

		USBillingServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-2" );
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
	{
		g_NexonNISMSServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-1" );

		g_NexonEUSessionServer.OnDestroy();
		LOG.PrintTimeAndLog( 0, "App Close 6-2" );

		NexonNISMSServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-3" );
		
		NexonEUSessionServer::ReleaseInstance();
		LOG.PrintTimeAndLog( 0, "App Close 6-4" );
	}

	//g_LogDBClient.
	LOG.PrintTimeAndLog( 0, "App Close 6-1" );
	//LogDBClient::ReleaseInstance();
	LOG.PrintTimeAndLog( 0, "App Close 6-2" );

	//	ThreadManager::GetInstance()->Clear();  //모든 쓰레드를 종료한다. //error 로그 kyg 
	LOG.PrintTimeAndLog( 0, "App Close 7" );

	//ThreadManager::ReleaseInstance();
	LOG.PrintTimeAndLog( 0, "App Close 8" );

	//ioPacketQueue::ReleaseInstance();
	LOG.PrintTimeAndLog( 0, "App Close 9" );

	//ServerNodeManager::ReleaseInstance();
	LOG.PrintTimeAndLog( 0, "App Close 10" );
	

	LOG.PrintTimeAndLog( 0, "App Close 11" );


	LOG.PrintTimeAndLog( 0, "App Close 12" );
	ioProcessChecker::ReleaseInstance();    

	LOG.PrintTimeAndLog( 0, "App Close Complete" );
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");

	ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	LOG.CloseLog();
	ProcessLOG.CloseLog();
	EndSocket();
	CoUninitialize();

	Sleep(300);

#if(_TEST)
	_exit(0);
#endif
	exit(1);
}


//yhr 테스오퍼레이션 용 임시함수
void ioMainProcess::SendCloseEvent()
{
	m_bReserveLogOut = true;
	m_dwReserveLogOutTime = TIMEGETTIME();
	
	m_bWantExit = true;
	
	//스케줄 노드 시간변경
	g_Manager.ChangeDeleteBillInfoScheduleTime();
}

bool ioMainProcess::InitNetWork()
{

	// 노드 관련 생성 후 네트웍 관련 생성
	g_RecvQueue.Initialize();

	g_ServerNodeManager.InitMemoryPool();
	g_MonitoringNodeManager.InitMemoryPool();

	g_ProcessChecker.LoadINI();

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		g_ThailandOTPNodeManager.InitMemoryPool();
	}
 
	// SET IP
	char szPrivateIP[16];
	char szPublicIP[16];
	GetPrivateProfileString("DEFAULT", "PrivateIP", "", szPrivateIP, sizeof(szPrivateIP), GetINI().c_str());
	GetPrivateProfileString("DEFAULT", "PublicIP", "", szPublicIP, sizeof(szPublicIP), GetINI().c_str());

	m_szPrivateIP = szPrivateIP;
	m_szPublicIP = szPublicIP;

	if (m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty())
	{
		return false;
	}


	m_iPort   = GetPrivateProfileInt("DEFAULT", "PORT", 1400,   GetINI().c_str());	
	m_iMSPort = GetPrivateProfileInt("DEFAULT", "MSPORT", 8409,   GetINI().c_str());
	
	
	g_App.CheckCreateNewLog( true );

	if(!g_iocp.Initialize()) 
	{
		LOG.PrintTimeAndLog(0,"[warning][main]IOCP FAILED:%d",GetLastError());
		LOG.PrintTimeAndLog(0,"[warning][main]Fail Billing Relay Server Socket.");
		return false;
	}

	if(!g_LogDBClient.ConnectTo())
	{
		LOG.PrintTimeAndLog(0,"[warning][main]LOGDB FAILED:%d",GetLastError());
		LOG.PrintTimeAndLog(0,"[warning][main]Fail connect LOGDB Agent.");
		return false;
	}

	g_MgrTool.InitMemoryPool();


	if( !IsTestMode() )
	{
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
 
			////g_ConnectAssist.PushNode(&g_MgameBillingServer);
 
			////g_ConnectAssist.PushNode(&g_ToonilandBillingServer);

			g_ConnectAssist.PushNode(&g_WemadeBuyServer);
			
			////g_ConnectAssist.PushNode(&g_NexonSessionServer); 
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
		{
			if( !g_ThailandLoginServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]Thailand Login FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect Thailand Login Server.");
				return false;
			}

			if( !g_ThailandIPBonusServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]Thailand IPBonus FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect Thailand IPBonus Server.");
				return false;
			}

			if( !g_ThailandIPBonusOutServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]Thailand IPBonus Out FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect Thailand IPBonus Out Server.");
				return false;
			}

			if( !g_ThailandBillingGetServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]Thailand Billing Get FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect Thailand Billing Get Server.");
				return false;
			}

			if( !g_ThailandBillingSetServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]Thailand Billing Set FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect Thailand Billing Set Server.");
				return false;
			}
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
		{
			if( !g_USBillingServer.ConnectTo( true ) )
			{
				LOG.PrintTimeAndLog(0,"[warning][main]US Billing FAILED:%d",GetLastError());
				LOG.PrintTimeAndLog(0,"[warning][main]Fail connect US Billing Server.");
				return false;
			}
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
		{
			g_ConnectAssist.PushNode(&g_NexonNISMSServer);
			g_ConnectAssist.PushNode(&g_NexonEUSessionServer);

			g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
			g_ThreadPool.Initialize();	
		}
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{ 
		//test 영역 
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		 
		if(IsTestMode())
		{
 			//g_ConnectAssist.PushNode(&g_ToonilandBillingServer);
 			g_ConnectAssist.PushNode(&g_WemadeBuyServer);
 			//g_ConnectAssist.PushNode(&g_MgameBillingServer);
			
			//g_ConnectAssist.PushNode(&g_NexonSessionServer);
		}

		g_ThreadPool.SetThreadPoolType( TPT_CHANNELING );
		g_ThreadPool.Initialize();

		g_NexonThreadPool.SetThreadPoolType( TPT_CHANNELING );
		g_NexonThreadPool.Initialize();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU ) 
	{
		LOG.PrintTimeAndLog(0,"[TESTMODE] LCT_EU");
				//test 영역 
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		
		if(IsTestMode())
		{
			g_ConnectAssist.PushNode(&g_NexonNISMSServer);
			g_ConnectAssist.PushNode(&g_NexonEUSessionServer);
		}
		

		
		g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
		g_ThreadPool.Initialize();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA )
	{
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
		g_ThreadPool.Initialize();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_LATIN )
	{
		LOG.PrintTimeAndLog(0,"[TESTMODE] LCT_LATIN");
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
		g_ThreadPool.Initialize();
	}
	else if(ioLocalManager::GetLocalType()==ioLocalManager::LCT_US)
	{
		LOG.PrintTimeAndLog(0,"[TESTMODE] LCT_US");
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
		g_ThreadPool.Initialize();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN )
	{
		LOG.PrintTimeAndLog(0,"[TESTMODE] LCT_TAIWAN");
#if(_TEST)
		TestThread* testThread = new TestThread;
		testThread->Begin();
#endif
		g_ThreadPool.SetThreadPoolType( TPT_LOCAL );
		g_ThreadPool.Initialize();

	}

	LogicThreadStart();
	 

	ServerBindStart();
	//모니터링 툴 입장 시작.
	g_App.MonitoringBindStart();

	MgrToolBindStart(); // 매니져 ㄱㄱ 

	if( !g_ConnectAssist.Begin() )
		LOG.PrintTimeAndLog(0,"ConnectAssist Fail!!");
 

	ioINILoader kLoader2( "config/sp2_auto_run.ini" );
	kLoader2.SaveBool( "config", "AutoRun", true );
	NexonPacketHeader ttt;
	int a= 0;
	return true;
}

bool ioMainProcess::Startup(const char* scriptName)
{
	SetINI(scriptName);

	const char* szINI = GetINI().c_str();
	GetPrivateProfileString("Default", "Log", "MLOG", m_szLogFolder, sizeof(m_szLogFolder), szINI);	


	if(!BeginSocket())
	{
		LOG.PrintTimeAndLog(0,"WSAStartup FAILED:%d",GetLastError());
		return false;
	}
	
	return true;
}


bool ioMainProcess::Initialize()
{
	// 다른 init 보다 먼저 할당
	m_pLocalMgr = new ioLocalManager;
	if( m_pLocalMgr )
		m_pLocalMgr->Init();

	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Local" );
	ioLocalManager::SetLocalType( (ioLocalManager::LocalType) kLoader.LoadInt( "Version", 0 ) );

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->Init();

	kLoader.SetTitle( "Test" );
	m_iTestMode =  GetPrivateProfileInt("Test", "TestMode",0,GetINI().c_str());

	LOG.PrintTimeAndLog( 0, "%s bTestMode=%d", __FUNCTION__, m_iTestMode );
	m_dwLogOutWaitTime = kLoader.LoadInt( "LogOutWaitTime", WAIT_TIME_FOR_RESERVE_LOGOUT );

	kLoader.SetTitle( "NETWORK" );
	m_bUseMonitoring = kLoader.LoadBool( "UseMonitoring", false );
	LOG.PrintTimeAndLog( 0, "%s UseMonitoring=%d", __FUNCTION__, (int) m_bUseMonitoring );

	kLoader.SetTitle( "TestEU" );
	m_iEUTestMode =  kLoader.LoadInt( "EUTestMode", 0 );
	kLoader.LoadString( "NexonTestPublicIP", "", m_EUTestPublicIP, MAX_PATH );
	

	m_pChannelingMgr = new ioChannelingNodeManager;

	if(m_pChannelingMgr)
		m_pChannelingMgr->Init();
	else
		return false;

 
	if(!InitNetWork()) return false;
	
	FrameTimer.Start( 30.0f );
	srand( timeGetTime() );

	HRESULT hRes = CoInitializeEx( NULL, COINIT_MULTITHREADED ); // 사용하기 위해서는 WIN32_DCOM을 프로젝트 속성에 전처리기 정의에 등록해야함.
	if (FAILED(hRes))
	{
		LOG.PrintTimeAndLog(0, "ioMainProcess::Initialize : Fail CoInitializeEx %d", GetLastError() );
		return false;
	}

	m_pGoodsMgr = new GoodsManager;
	if( m_pGoodsMgr )
	{
		if( !m_pGoodsMgr->LoadINI( "config/GoodsList.ini", "config/GoodsName.ini" ) )
			return false;
	}
	else
		return false;

	return true;
}

void ioMainProcess::ServerBindStart()
{
	if( m_bWantExit || m_bReserveLogOut ) return;
	if( m_pServerBind ) return;

	m_pServerBind = new ioServerBind;

	if(m_pServerBind == NULL ) return; 

	if( !m_pServerBind->Start( (char*)m_szPrivateIP.c_str(), m_iPort ) )
	{
		g_App.SetWantExit();
		return;
	}

	char strText[MAX_PATH] = "";
	sprintf_s( strText, "%s:%s:%d", m_szPublicIP.c_str(), m_szPrivateIP.c_str(), m_iPort );
 
}

void ioMainProcess::MonitoringBindStart()
{
	if( m_bWantExit || m_bReserveLogOut ) return;
	if( m_pMonitoringBind ) return;
	if( !m_bUseMonitoring ) return;

	m_pMonitoringBind = new ioMonitoringBind;

	if(m_pMonitoringBind == NULL) return;

	if( !m_pMonitoringBind->Start( "127.0.0.1", m_iMSPort ) ) // 모니터링 클라이언트 같은 서버에서 실행됨
		return;
}

void ioMainProcess::MgrToolBindStart()
{
	if( m_bWantExit || m_bReserveLogOut ) return;
	if( m_MgrToolBind ) return;
 

	const char* szINI = GetINI().c_str();
	m_iMgrPort = GetPrivateProfileInt("DEFAULT", "ManagerToolPort", 9000, szINI);	

	m_MgrToolBind = new ioMgrToolBind;
	if( !m_MgrToolBind->Start(m_szPublicIP.c_str() , m_iMgrPort ) ) // 모니터링 클라이언트 같은 서버에서 실행됨
		return;
}

void ioMainProcess::LogicThreadStart()
{
	m_pGameLogicThread = new LogicThread;

	if(m_pGameLogicThread)
	{
		m_pGameLogicThread->SetProcessor(this);
		m_pGameLogicThread->Begin();
	}
}

void ioMainProcess::Process(uint32& idleTime)
{
	g_ProcessChecker.MainThreadCheckTimeStart();
	g_ProcessChecker.CallMainThread();
	FrameTimer.SetFrame();

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		g_ThailandOTPNodeManager.Node_Destroy();
	}
	
	// 모든 패킷의 처리
	int iPacketParsingSize = g_RecvQueue.PacketParsing();
	g_ProcessChecker.MainProcessMaxPacket( iPacketParsingSize );

	// 프로세스.
	g_ServerNodeManager.ProcessServerNode();

	g_MgrTool.ProcessMgrToolNode();

	if( !IsTestMode() )
	{
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			////g_MgameBillingServer.ProcessTime();
			////g_ToonilandBillingServer.ProcessTime();
			g_WemadeBuyServer.ProcessTime();
		//	g_NexonSessionServer.ProcessTime();
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
		{
			g_ThailandLoginServer.ProcessTime();
			g_ThailandOTPNodeManager.Process();
			g_ThailandIPBonusServer.ProcessTime();
			g_ThailandIPBonusOutServer.ProcessTime();
			g_ThailandBillingGetServer.ProcessTime();
			g_ThailandBillingSetServer.ProcessTime();
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
		{
			g_USBillingServer.ProcessTime();
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
		{
			g_NexonNISMSServer.ProcessTime();
			g_NexonEUSessionServer.ProcessTime();
		}
	}
	if(IsTestMode())
	{ 
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			//g_ToonilandBillingServer.ProcessTime();//kyg test 때문에 켜논거 꼭 꺼야함 
			//g_MgameBillingServer.ProcessTime(); //kyg test 때문에 켜논거 꼭 꺼야함 
			g_WemadeBuyServer.ProcessTime();//kyg test 때문에 켜논거 꼭 꺼야함 
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
		{
			g_NexonNISMSServer.ProcessTime();
			g_NexonEUSessionServer.ProcessTime();
		}
		else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
		{
			g_USBillingServer.ProcessTime();
		}
	}
 
	g_LogDBClient.ProcessTime();

	ProcessTime();

	g_ProcessChecker.Process();
	g_ProcessChecker.MainThreadCheckTimeEnd();

	if(iPacketParsingSize == 0)
	{
		idleTime = 1;
	}
	else
		idleTime = 0;
}

void ioMainProcess::Draw()
{
#if 0 
	if( !m_bInfoDraw )
		return;

	if( TIMEGETTIME() - m_dwDrawTimer < 1000 ) return;          //1초에 한번만 찍는다.

	m_dwDrawTimer = TIMEGETTIME();

	PAINTSTRUCT ps;


	int iYPos = 0;
	enum { LINE_SPACE = 14, };

	char strText[MAX_PATH] = "";
	
	//GLOBAL TIME
	sprintf_s(strText,"GLOBAL TIME : %d",TIMEGETTIME());
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Network Info
	sprintf_s(strText,"SERVER IP:%s:%s PORT:%d MSPORT:%d", m_szPublicIP.c_str(), m_szPrivateIP.c_str(), m_iPort, m_iMSPort );
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Thread Info
	sprintf_s(strText,"THREAD COUNT: %d",ThreadManager::GetInstance()->GetHandleCount());
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Connect Client Info
	sprintf_s(strText,"JOIN SERVER: %d(%d)        ",g_ServerNodeManager.GetNodeSize(), g_ServerNodeManager.GetDestroyNodeSize());
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Remainder MemPool Info
	sprintf_s(strText,"REMAINDER MEMPOOL: %d ServerMemCount",g_ServerNodeManager.RemainderNode());
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//RECV QUEUE
	int usingCount[4], remainCount[4];
	g_RecvQueue.GetPoolCount( usingCount, remainCount );
	sprintf_s(strText,"RECV PACKET: %d:%d:%d:%d QUEUE", usingCount[0], usingCount[1], usingCount[2], usingCount[3] );
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Remainder MemPool Info
	sprintf_s(strText,"REMAINDER MEMPOOL: %d:%d:%d:%d PacketMemCount", remainCount[0], remainCount[1], remainCount[2], remainCount[3] );
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;

	//Log SERVER INFO
	if( g_LogDBClient.IsActive() )
		sprintf_s(strText,"LOG DB SERVER CONNECT");
	else
		sprintf_s(strText,"LOG DB SERVER DISCONNECT");
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;


	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		//Mgame SERVER INFO
		if( g_MgameBillingServer.IsActive() )
			sprintf_s(strText,"Mgame Billing SERVER CONNECT");
		else
			sprintf_s(strText,"Mgame Billing SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		//WEMADE SERVER INFO
 
		if( g_WemadeBillingServer.IsActive() )
			sprintf_s(strText,"Wemade Billing SERVER CONNECT");
		else
			sprintf_s(strText,"Wemade Billing SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
  
		//WEMADE BUY SERVER INFO
		if( g_WemadeBuyServer.IsActive() )
			sprintf_s(strText,"Wemade Buy SERVER CONNECT");
		else
			sprintf_s(strText,"Wemade Buy SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		//Tooniland SERVER INFO
		if( g_ToonilandBillingServer.IsActive() )
			sprintf_s(strText,"Tooniland Billing SERVER CONNECT");
		else
			sprintf_s(strText,"Tooniland Billing SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;


		sprintf_s(strText,"UserInfo memPool: %d/%d ",g_UserInfoManager->GetMemoryPoolCount(),g_UserInfoManager->GetUserInfoCount());//kyg 여기에 유저인퐄운트 추가 
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
		//Tooniland SERVER INFO
 
		if( g_NexonSessionServer.IsActive() )
			sprintf_s(strText,"Nexon Session SERVER CONNECT");
		else
			sprintf_s(strText,"Nexon Session  SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
 
		// Thread Pool
		sprintf_s(strText,"DataPool:%d(%d) : ActiveThread:%d/%d", g_ThreadPool.GetNodeSize(), g_ThreadPool.RemainderNode(), g_ThreadPool.GetActiveThreadCount(), g_ThreadPool.GetThreadCount() );
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		//Remainder MemPool Info
		if( g_ThailandLoginServer.IsActive() )
			sprintf_s(strText,"THAILAND LOGIN SERVER CONNECT");
		else
			sprintf_s(strText,"THAILAND LOGIN SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		if( g_ThailandIPBonusServer.IsActive() )
			sprintf_s(strText,"THAILAND IPBONUS SERVER CONNECT");
		else
			sprintf_s(strText,"THAILAND IPBONUS SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		if( g_ThailandIPBonusOutServer.IsActive() )
			sprintf_s(strText,"THAILAND IPBONUS OUT SERVER CONNECT");
		else
			sprintf_s(strText,"THAILAND IPBONUS OUT SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		if( g_ThailandBillingGetServer.IsActive() )
			sprintf_s(strText,"THAILAND BILLING GET SERVER CONNECT");
		else
			sprintf_s(strText,"THAILAND BILLING GET SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		if( g_ThailandBillingSetServer.IsActive() )
			sprintf_s(strText,"THAILAND BILLING SET SERVER CONNECT");
		else
			sprintf_s(strText,"THAILAND BILLING SET SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		sprintf_s(strText,"THAILAND OTP: %d(%d)",g_ThailandOTPNodeManager.GetNodeSize(), g_ThailandOTPNodeManager.RemainderNode() );
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			sprintf_s(strText,"THAILAND User: %d(%d)", pLocal->GetThirdNodeSize(), pLocal->RemainderThirdNode() );
			TextOut(hdc,0,iYPos,strText,strlen(strText));
			iYPos += LINE_SPACE;

			sprintf_s(strText,"THAILAND Login Info: %d(%d)", pLocal->GetSecondNodeSize(), pLocal->RemainderSecondNode() );
			TextOut(hdc,0,iYPos,strText,strlen(strText));
			iYPos += LINE_SPACE;

			sprintf_s(strText,"THAILAND OTP Info: %d(%d)", pLocal->GetNodeSize(), pLocal->RemainderNode() );
			TextOut(hdc,0,iYPos,strText,strlen(strText));
			iYPos += LINE_SPACE;
		}
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
	{
		if( g_USBillingServer.IsActive() )
			sprintf_s(strText,"US BILLING SERVER CONNECT");
		else
			sprintf_s(strText,"US BILLING SERVER DISCONNECT");
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
	}

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA || 
	    ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN    ||
		ioLocalManager::GetLocalType() == ioLocalManager::LCT_US         )
	{
		sprintf_s(strText,"LOGIN: LoginInfoSize : %d(%d)", g_ThreadPool.GetLoginNodeSize(), g_ThreadPool.RemainderLoginNode() );
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;

		// Thread Pool
		sprintf_s(strText,"DataPool:%d(%d) : ActiveThread:%d/%d", g_ThreadPool.GetNodeSize(), g_ThreadPool.RemainderNode(), g_ThreadPool.GetActiveThreadCount(), g_ThreadPool.GetThreadCount() );
		TextOut(hdc,0,iYPos,strText,strlen(strText));
		iYPos += LINE_SPACE;
	}

	sprintf_s(strText,"VERSION: %s | %s", STRFILEVER, STRINTERNALNAME );
	TextOut(hdc,0,iYPos,strText,strlen(strText));
	iYPos += LINE_SPACE;
	
	if(m_bReserveLogOut)
	{
		DWORD dwGapTime = TIMEGETTIME()-m_dwReserveLogOutTime;
		if(m_dwLogOutWaitTime > dwGapTime)
		{
			StringCbPrintf(strText, sizeof(strText), "EXIT AFTER %03d S", (m_dwLogOutWaitTime-dwGapTime)/1000 );
			SetTextColor(hdc, RGB(255,0,0));
			TextOut(hdc,0,iYPos,strText,strlen(strText));
			iYPos += LINE_SPACE;
		}
		
	}
#endif
 	
}

void ioMainProcess::SetWantExit()
{
	if( !m_bWantExit && !m_bReserveLogOut )
	{
		ioINILoader kLoader( "config/sp2_auto_run.ini" );
		kLoader.SaveBool( "config", "AutoRun", false );

		m_bInfoDraw = true;
		m_bReserveLogOut = true;
		m_dwReserveLogOutTime = TIMEGETTIME();
		m_bWantExit = true;

		//스케줄 노드 시간변경
		g_Manager.ChangeDeleteBillInfoScheduleTime();

	}
	while(1) //kyg 서비스 종료 시키지만 빌링 서버는 ini 설정된 값만큼 살아있어야함 
		Sleep(1000);
	 
}

void ioMainProcess::CrashDown()
{
	
	m_bInfoDraw = true;
	m_bReserveLogOut = true;
	m_dwReserveLogOutTime = TIMEGETTIME();
	m_bWantExit = true;

	SAFEDELETE( m_pServerBind );         //서버를 종료하므로 더이상의 Accept를 받지 않는다.	
	SAFEDELETE(m_pMonitoringBind);

	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		////g_MgameBillingServer.OnDestroy();
		g_WemadeBuyServer.OnDestroy();
		////g_ToonilandBillingServer.OnDestroy();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_THAILAND )
	{
		g_ThailandLoginServer.OnDestroy();
		g_ThailandIPBonusServer.OnDestroy();
		g_ThailandIPBonusOutServer.OnDestroy();
		g_ThailandBillingGetServer.OnDestroy();
		g_ThailandBillingSetServer.OnDestroy();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_US )
	{
		g_USBillingServer.OnDestroy();
	}
	else if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_EU )
	{
		g_NexonNISMSServer.OnDestroy();
		g_NexonEUSessionServer.OnDestroy();
	}
}
 

void ioMainProcess::CheckCreateNewLog( bool bStart )
{
	static char szPrevTime[MAX_PATH] = "";

	SYSTEMTIME st;
	GetLocalTime(&st);
	char szCurTime[MAX_PATH] = "";
	StringCbPrintf(szCurTime, sizeof(szCurTime), "%04d%02d%02d", st.wYear, st.wMonth, st.wDay);

	if( strcmp(szCurTime, szPrevTime) != 0)
	{
		memset(szPrevTime, 0, sizeof(szPrevTime));
		StringCbCopy(szPrevTime, sizeof(szPrevTime), szCurTime);

		char TimeLogName[MAX_PATH]="";
		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "MLOG\\BUG%s-%s-%d.log", szCurTime,m_szPrivateIP.c_str(),m_iPort);
		{
//			LogSync ls( &LOG ); //kyg 이제 필요 없는 기능 
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			LOG.CloseLog();
			LOG.OpenLog(0, TimeLogName, true);
			LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			LOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		}

		memset(TimeLogName, 0, sizeof(TimeLogName));
		StringCbPrintf(TimeLogName, sizeof(TimeLogName), "MLOG\\PROCESS%s-%s-%d.log", szCurTime,m_szPrivateIP.c_str(),m_iPort );
		{
			//LogSync ls( &ProcessLOG ); //kyg 이제 필요 없는 기능 
			ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			ProcessLOG.CloseLog();
			ProcessLOG.OpenLog(0, TimeLogName, true);
			ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
			ProcessLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
			ProcessLOG.PrintTimeAndLog( 0, "[Main][LogDB][GET][OUTPUT][IP][LOGIN]" );
		}

		//memset(TimeLogName, 0, sizeof(TimeLogName));
		//StringCbPrintf(TimeLogName, sizeof(TimeLogName), "MLOG\\NET%s.log", szCurTime);
		//{
		//	LogSync ls( &NetLOG );
		//	NetLOG.CloseLog();
		//	NetLOG.OpenLog(0, TimeLogName, true);
		//	NetLOG.PrintTimeAndLog( 0, "Version : %s | %s", STRFILEVER, STRINTERNALNAME );
		//}
	}
}


void ioMainProcess::ProcessTime()
{
	
	if(m_bReserveLogOut)
	{
		if(m_dwReserveLogOutTime != 0)
		{
			if(TIMEGETTIME() - m_dwReserveLogOutTime > m_dwLogOutWaitTime )
			{
//				::SendMessage( GetHwnd(), WM_DESTROY, 0, 0 );
				m_dwReserveLogOutTime = 0;
				Destroy();

				return;

			}
		}
		char strText[256];

		DWORD dwGapTime = TIMEGETTIME()-m_dwReserveLogOutTime;
		
		if(m_dwLogOutWaitTime > dwGapTime &&  TIMEGETTIME() - m_dwDrawTimer > 1000)
		{
			
			m_dwDrawTimer = TIMEGETTIME();
			
			int remainderProcessCount = g_BillInfoManager->GetCount() + g_ThreadPool.GetNodeSize() + g_NexonThreadPool.GetNodeSize();
										
			g_BillInfoManager->RemainBillInfoCount();

			StringCbPrintf(strText, sizeof(strText), "EXIT AFTER %03d S RequestCount(%d)", (m_dwLogOutWaitTime-dwGapTime)/1000,remainderProcessCount );
			PrintTimeAndLog(0,strText);

			if(remainderProcessCount == 0 )
			{
				LOG.PrintTimeAndLog(0,"Request All Complete ");
				m_dwLogOutWaitTime = 1;
			}
		}


	}

	if( TIMEGETTIME() - m_dwCurTime < 60000 ) return;

	CheckCreateNewLog();

	m_dwCurTime = TIMEGETTIME();
}


bool ioMainProcess::IsTestMode()
{
	if( m_iTestMode > 0 )
		return true;

	return false;
}

char *ioMainProcess::GeEUTestPublicIP()
{
	return m_EUTestPublicIP;
}

bool ioMainProcess::IsEuTestMode()
{
	if( m_iEUTestMode > 0 )
	{
		return true;
	}

	return false;
}

void ioMainProcess::PrintTimeAndLog( int debuglv, LPSTR fmt )
{
	LOG.PrintTimeAndLog(debuglv,fmt);
}

void ioMainProcess::DebugLog( int debuglv, LPSTR filename, int linenum, LPSTR fmt )
{
	LOG.DebugLog(debuglv,filename,linenum,fmt);
}
