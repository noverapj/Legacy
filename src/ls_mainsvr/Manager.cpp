#include "StdAfx.h"
#include "MainProcess.h"
#include "Shutdown.h"
#include "Manager.h"
#include "Scheduler\SchedulerNode.h"

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

	if(!Prepare())
		return FALSE;

	Timer();
	return TRUE;
}

void Manager::Startup(const char* scriptName)
{
	timeBeginPeriod(1);

	g_MainProc.SetINI(scriptName);
}

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
			g_MainProc.Shutdown(SHUTDOWN_QUICK); 
			return FALSE;
	} 
	return TRUE;
}	

BOOL Manager::Prepare()
{
	/*종료시 저장하는함수 호출하도록 설정해줌*/ 
    if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE) ) 
		return FALSE; 

	if( ! g_MainProc.Initialize() )
		return FALSE;

	// scheduler
	m_scheduler	= new SchedulerNode;
	InitScheduler();

	FrameTimer.Start( 30.0f );
	srand( timeGetTime() );

	return TRUE;
}

void Manager::InitScheduler()
{
	m_scheduler->OnCreate();
	m_scheduler->AddSchedule( ITPK_FLUSH_PROCESS, g_MainProc.GetNagleTime() );
	m_scheduler->AddSchedule( ITPK_TIMEOUT_TRADE, 1000 * 60 * 3 );
	//m_scheduler->AddSchedule( ITPK_QUERY, 10 );
	m_scheduler->AddSchedule( ITPK_SPECIAL_SHOP_CHECK, 1000 * 60 * 1 );
	m_scheduler->AddSchedule( ITPK_MATCH_PROCESS, 1000 * g_MainProc.GetMatchingTime() );
	m_scheduler->AddSchedule( ITPK_MATCH_RANKSEASON, 1000 * 10 );
}

void Manager::Timer()
{
	while(TRUE)
	{
		m_scheduler->Begin();
		while( TRUE )
		{
			Schedule* pSchedule = m_scheduler->GetSchedule();
			if( pSchedule )
			{
				m_scheduler->Call( pSchedule->GetCommand() );
			}
			else
			{
				break;
			}
		}
		m_scheduler->End();

		Sleep( 1 );
	}
}
