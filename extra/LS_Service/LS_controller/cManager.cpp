#include "stdafx.h"
#include "cProcessor.h"
#include "cEnvironment.h"
#include "cScheduler.h"
#include "cManager.h"
#include "../include/Constant.h"
#include "../include/ISystem.h"
#include "../include/cSingleton.h"
#include "../common/Internal.h"

cManager::cManager(void)
{
}

cManager::~cManager(void)
{
}

BOOL cManager::Init(const uint32 maxSession, const uint32 flush)
{
	g_environment.SetMaxSession(maxSession);
	g_environment.SetFlushTime(flush);

	cSingleton<ISystem>::GetInstance()->SetProcessor(cSingleton<cProcessor>::GetInstance());
	cSingleton<ISystem>::GetInstance()->SetEvent(I_CONNECT, I_CLOSE);

	if(!cSingleton<ISystem>::GetInstance()->Startup(maxSession, 1024*51, 1000, 0))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL cManager::Listen(const uint16 port, const uint32 backlog)
{
	if(!cSingleton<ISystem>::GetInstance()->Listen(port, backlog))	
	{
		return FALSE;
	}
	return TRUE;
}

BOOL cManager::Start(const uint32 worker, const uint32 commander)
{
	if(!cSingleton<ISystem>::GetInstance()->Start(worker, commander, 0))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL cManager::Timer()
{
	// sets the minimum timer resolution for an application or device driver.
	timeBeginPeriod(1);

	g_scheduler.AddSchedule(I_PING, NULL, 0, 100000);								// ping ют╥б
	g_scheduler.AddSchedule(I_FLUSH, NULL, 0, g_environment.GetFlushTime());		// flush out

	cSchedule* schedule = NULL;
	while(TRUE)
	{
		if(g_scheduler.Begin())
		{
			while((schedule = g_scheduler.GetSchedule()))
			{
				cSingleton<ISystem>::GetInstance()->Call(
					schedule->GetCommand(),
					NULL,
					0);
			}

			g_scheduler.End();
		} 
		Sleep(33);
	}

	// clears a previously set minimum timer resolution. 
	timeEndPeriod(1);
	return TRUE;
}
