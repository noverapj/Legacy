#include "StdAfx.h"
#include "cScheduler.h"


cScheduler g_scheduler;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cScheduler::cScheduler() : m_current(0)
{
	Init();
}

cScheduler::~cScheduler(void)
{
	Destroy();
}

void cScheduler::Init()
{
}

void cScheduler::Destroy()
{
	SCHEDULES::iterator it;
	for(it = m_schedules.begin() ; it != m_schedules.end() ; ++it)
	{
		delete (*it);
	}
	m_schedules.clear();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL cScheduler::Begin()
{
	m_iterator	= m_schedules.begin();
	m_current	= GetTickCount();
	return TRUE;
}

void cScheduler::End()
{
	//Debug("cScheduler::End() ÃÑ ¼Ò¿ä½Ã°£ : %lu\n", GetTickCount()-m_current);
}

BOOL cScheduler::AddSchedule(const uint32 command, const uint32 data, const uint32 size, const uint32 tick)
{
	m_schedules.push_back(new cSchedule(command,data,size,tick));
	return TRUE;
}

cSchedule* cScheduler::GetSchedule()
{
	cSchedule* schedule = NULL;
	while(m_iterator != m_schedules.end())
	{
		schedule = (*m_iterator);
		++m_iterator;

		if(schedule->IsTimeup(m_current))
		{
			return schedule;
		}
	}
	return NULL;
}