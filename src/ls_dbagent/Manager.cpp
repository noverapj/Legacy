#include "StdAfx.h"
#include "MainProcess.h"
#include "ThreadPool/ioThreadPool.h"
#include "Network/DBServer.h"
#include "Network/iocpHandler.h"
#include "Network/ioPacketQueue.h"
#include "Network/ioLogServer.h"
#include "NodeInfo/UserNodeManager.h"
#include "NodeInfo/LogNodeManager.h"
#include "Database/cQueryManager.h"
#include "Manager.h"
#include "../ioINILoader/ioINILoader.h"


BOOL ConsoleHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 
	case CTRL_C_EVENT: 
	case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit. 
	case CTRL_BREAK_EVENT: 
	case CTRL_LOGOFF_EVENT: 
	case CTRL_SHUTDOWN_EVENT: 
	default: 
	 g_MainProc.Save();
		return FALSE;
	} 
	return TRUE;
}	

Manager::Manager(void)
{
	Init();
}

Manager::~Manager(void)
{
	Destroy();
}

void Manager::Init()
{
}

void Manager::Destroy()
{
}

BOOL Manager::Run(const char* scriptName)
{
	Startup(scriptName);

	if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE) ) 
		return FALSE; 
		
	if(!LoadINI())			return FALSE;
	if(!CreateDB())			return FALSE;
	if(!ListenNetwork())	return FALSE;
	if(!CreateUserPool())	return FALSE;
	if(!Prepare())			return FALSE;
	if(!StartModules())		return FALSE;

	Timer();
	return TRUE;
}

void Manager::Startup(const char* scriptName)
{
	timeBeginPeriod(1);

	g_MainProc.SetINI(scriptName);
}

BOOL Manager::LoadINI()
{
	//---------------------------------
	//        ini 정보
	//---------------------------------
	const char* szINI = g_MainProc.GetINI().c_str();

	m_logSvrPort = GetPrivateProfileInt("Default", "LogServerPort", 0, szINI);	 //로그 서버별로 
	(m_logSvrPort == 0) ? g_MainProc.SetLoggerState(FALSE) : g_MainProc.SetLoggerState(TRUE);

	m_port = GetPrivateProfileInt("Default", "Port", 10000, szINI);	
	TCHAR szLog[256];
	GetPrivateProfileString("Default", "Log", "MLOG", szLog, sizeof(szLog), szINI);	
	g_MainProc.SetLogFolder( szLog );

	if(g_MainProc.GetLoggerState() == FALSE)
	{
		GetPrivateProfileString("SQL", "IP", "", m_dbIP, sizeof(m_dbIP), szINI);	
		GetPrivateProfileString("SQL", "Database", "", m_dbName, sizeof(m_dbName), szINI);	
		GetPrivateProfileString("SQL", "ID", "", m_dbID, sizeof(m_dbID), szINI);	
		GetPrivateProfileString("SQL", "PW", "", m_dbPwd, sizeof(m_dbPwd), szINI);	

		m_dbPort = GetPrivateProfileInt("SQL", "Port", 1433, szINI);	

		if(strcmp(m_dbIP,"") == 0)
		{
			m_error = 0x1000;
			return FALSE;
		}
	}
	g_MainProc.LogCurrentStates( 0 );
	return TRUE;
}

BOOL Manager::CreateDB()
{
	if(g_MainProc.GetLoggerState())
		return TRUE;
	 
	Information( "Create DatabasePool.." );
	//--------------------------------
	//     DB 접속
	//--------------------------------

	if(!g_MainProc.LoadQuery())
	{
		m_error = 0x1002;
		return FALSE;
	}

	if(!g_queryManager.AddDatabase(1, m_dbIP, m_dbPort, m_dbName, m_dbID, m_dbPwd))
	{
		m_error = 0x1002;
		return FALSE;
	}
	Information( "done\n" );
	return TRUE;
}

BOOL Manager::CreateUserPool()
{
	if(!g_MainProc.GetLoggerState())
		g_UserNodeManager.InitMemoryPool();
	else
		g_LogNodeManager->InitMemoryPool();
	return TRUE;
}

BOOL Manager::ListenNetwork()
{
	
	g_RecvQueue.Initialize();

	//
	m_logicThread = new LogicThread;
	m_logicThread->SetProcessor(&(g_MainProc));

	// 메인프로세스와 로직스레드 구동
	if( !g_iocp.Initialize() )
	{
		m_error = 0x1003;
		return FALSE;
	}

	//--------------------------------
	//     SOCKET Init
	//--------------------------------
	if( !BeginSocket() )
	{
		m_error = 0x1004;
		return FALSE;
	}

	// SET IP
	char szCurrentPath[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH, szCurrentPath);
	strcat_s(szCurrentPath, "\\ls_config_dba.ini");

	char szPrivateIP[16];
	char szPublicIP[16];
	GetPrivateProfileString("NETWORK", "PrivateIP", "", szPrivateIP, sizeof(szPrivateIP), szCurrentPath);
	GetPrivateProfileString("NETWORK", "PublicIP", "", szPublicIP, sizeof(szPublicIP), szCurrentPath);

	m_szPrivateIP = szPrivateIP;
	m_szPublicIP = szPublicIP;

	if (m_szPrivateIP.IsEmpty() || m_szPublicIP.IsEmpty())
	{
		m_error = 0x1005;
		return FALSE;
	}
	
	Information( "Listening TCP .." );

	if(!g_MainProc.GetLoggerState())
	{
		m_pDBServer = new ioDBServer; 
		if( !m_pDBServer->Start( (char*)m_szPrivateIP.c_str(), m_port ) )
		{
			Information( "failed\n" );
			m_error = 0x1006;
			return FALSE;
		}
	}

	else 
	{
		m_pLogServer = new ioLogServer;
		if( !m_pLogServer->Start( (char*)m_szPrivateIP.c_str(), m_logSvrPort) )
		{
			Information( "fialed\n");
			m_error = 0x1006;
			return FALSE;
		}
		LOG.PrintTimeAndLog(0,"Logger(%s:%d) Start done",m_szPrivateIP.c_str(),m_logSvrPort);
	}
	Information( "done\n" );

	return TRUE;
}

BOOL Manager::StartModules()
{
	if(!g_MainProc.GetLoggerState())
	{
		Information( "Database ThreadPool Start..." );
		if(g_ThreadPool.Initialize() == FALSE)
		{
			Information( "failed\n" );
			m_error = 0x1008;
			return FALSE;
		}
		Information( "done\n" );
	}

	Information( "Starting processor..." );
	if(!m_logicThread->Begin())
	{
		Information( "failed\n" );
		return FALSE;
	}
	Information( "done\n" );

	return TRUE;
}

BOOL Manager::Prepare()
{
	char szTitle[512] ="";
	if(!g_MainProc.GetLoggerState())
	{
		wsprintf( szTitle, "%s(%s) %s:%d", m_dbName, m_dbIP, m_szPrivateIP.c_str(), m_port );
	}
	else
	{
		wsprintf( szTitle, "ls_logger %s:%d", m_szPrivateIP.c_str(), m_port );
	}
	
	if(!g_MainProc.Initialize( szTitle ))
	{
		m_error = 0x1001;
		return FALSE;
	}

	return TRUE;
}

void Manager::Timer()
{
	Information( " >> Server Started\n" );
	while(TRUE)
	{
		Sleep(10000);
	}
}
