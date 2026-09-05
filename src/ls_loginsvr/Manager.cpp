#include "StdAfx.h"
#include "Manager.h"
#include "UserDefineSingleton.h"

 
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
	//	ShutDown();
	 
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			LOG.CloseLog();
			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
			ReportLOG.CloseLog();
			Sleep(100);
	 
		return FALSE;
	} 
	return TRUE;
}	

Manager::Manager(void)
{
}


Manager::~Manager(void)
{
}
 
bool Manager::Init()
{

#if(1) //kyg 이거 꼭 살릴것
  //  S_Test::instance()->Run();
#endif
	timeBeginPeriod(1);

	if(!g_Config()->Init())
		return false;

	g_Queue()->Initz();

	g_Iocp()->Init(g_Config()->NWorkerCount());
	g_ClientMgr()->InitMemoryPool();
	g_TimerMgr()->SetQueue(g_Queue());
	g_TimerMgr()->SetPool(g_OPMemPool());
	g_Logic()->Init();
	g_State()->SetOPoolCount(g_OPMemPool()->GetSize());
	g_LoginServerInfo()->Init();
 
	if(!g_LoginServerInfo()->Start(g_Config()->SIpAddr().c_str(),g_Config()->NPort()))
	{
		LOG.PrintTimeAndLog(0,"LoginServer Acceptor Bind Error\n");
		return false;
	}

	g_ServerInfoMgr();
	g_ConnectAssist()->Init();
	g_ConnectAssist()->Begin();
	g_ServerConnectMgr()->Connect(g_Config()->ServerAddr());
 	return true;
}

bool Manager::Run(TCHAR* scriptName)
{
	g_Config()->SetINI(scriptName);
	try
	{
		if(SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE)
			return false;
		if(!Init())
		{
			LOG.PrintTimeAndLog(0,"초기화 실패로 종료 로그를 확인하세요\n");
			return false;
		}
	}
	catch(_com_error & e)
	{
		LOG.PrintTimeAndLog(0,"Manager Failed");
		std::cout<<e.ErrorMessage();
	}
	return true;
}