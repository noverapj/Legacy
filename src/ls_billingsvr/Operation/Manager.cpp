#include "../StdAfx.h"
#include "Manager.h"
#include "SchedulerNode.h"
#include "../Local/ioLocalManager.h"

extern CLog LOG;

#define CHANGE_SCHEDULER_TIME 10000
#define CHANGE_ALIVE_TIME 5000

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
	m_scheduler				= new SchedulerNode;
	if(m_scheduler == NULL)
	{
		LOG.PrintTimeAndLog(0,"error Manager::Init() ");
		exit(1);
	}
}

void Manager::Destroy()
{
}

BOOL Manager::Run(const char* scriptName)
{
	if(!Startup(scriptName))
		return FALSE;

	if(!Prepare())
		return FALSE;

	Timer();
	return TRUE;
}
 
BOOL Manager::Startup(const char* scriptName)
{
	timeBeginPeriod(1);

	SetHeapInformation();			// LFH

	if(!g_App.Startup(scriptName))
		return FALSE;

	return TRUE;
}

void Manager::DisplayHeapInfo(HANDLE heap) 
{ 
	ULONG heapInfo; 
	SIZE_T size; 

	if( HeapQueryInformation( heap, HeapCompatibilityInformation, &heapInfo, sizeof(heapInfo), &size ) == FALSE ) 
	{ 
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SetHeapInformation Failed" );
	} 
} 

void Manager::SetHeapInformation()
{
	HANDLE heaps[1025];
	BOOL results[1025];

	ZeroMemory( heaps, sizeof( heaps ) );
	ZeroMemory( results, sizeof( results ) );

	ULONG HeapFragValue = 2;

	DWORD count = GetProcessHeaps( 1024, heaps );
	for(DWORD i = 0; i < count; i++)
	{
		results[i] = HeapSetInformation( heaps[i], HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue) ); 
		if( !results[i] )
		{
			DisplayHeapInfo( heaps[i] );
		}
	}
}


BOOL Manager::Prepare()
{
	/*종료시 저장하는함수 호출하도록 설정해줌*/ 
	// scheduler
	if( ! m_scheduler )
		return FALSE;

	if( ! g_App.Initialize() )
		return FALSE;

	// init
	InitScheduler();

	return TRUE;
}

void Manager::InitScheduler()
{
	m_scheduler->OnCreate(); 
 
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		m_scheduler->AddSchedule(ITPK_NEXONALIVE, 60000);
		m_scheduler->AddSchedule(ITPK_NEXON_SESSION_CHECK, 30000);
	}

	m_scheduler->AddSchedule(ITPK_WRITEPROCESSLOG, 600000);
	m_scheduler->AddSchedule(ITPK_TIMEOUT_BILLINFO, 30000);;
	 
}

void Manager::ChangeDeleteBillInfoScheduleTime()
{	
	m_scheduler->ChangeTickValue(ITPK_TIMEOUT_BILLINFO, CHANGE_SCHEDULER_TIME);
	m_scheduler->m_BillMgrTimeout = CHANGE_ALIVE_TIME;
	
	LOG.PrintTimeAndLog(0, "Manager::ChangeDeleteBillInfoScheduleTime():%d", m_scheduler->m_BillMgrTimeout);
}

void Manager::Timer()
{
	LOG.PrintTimeAndLog(0,"Timer Thread :%d\n",GetCurrentThreadId());
	while( TRUE )
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

		Sleep(1);
	}
}