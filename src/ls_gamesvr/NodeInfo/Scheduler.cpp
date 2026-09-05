#include "stdafx.h"
#include "Scheduler.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scheduler::Scheduler() : m_current(0)
{
	Init();
}

Scheduler::~Scheduler(void)
{
	Destroy();
}

void Scheduler::Init()
{
}

void Scheduler::Destroy()
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

BOOL Scheduler::Begin()
{
	m_iterator	= m_schedules.begin();
	m_current	= GetTickCount();
	return TRUE;
}

void Scheduler::End()
{
	//Debug(_T("Scheduler::End() ÃÑ ¼Ò¿ä½Ã°£ : %lu\r\n"), GetTickCount()-m_current);
}

BOOL Scheduler::AddSchedule(const uint32 command, const uint32 tick)
{
	m_schedules.push_back(new Schedule(command, tick));
	return TRUE;
}

BOOL Scheduler::ChangeTickValue( const uint32 command, const uint32 tick )
{
	std::list<Schedule*>::iterator	iter	= m_schedules.begin();
	std::list<Schedule*>::iterator	iterEnd	= m_schedules.end();

	for( iter ; iter != iterEnd ; ++iter )
	{
		if( (*iter)->GetCommand() == command )
		{
			(*iter)->SetTick( tick );
			return TRUE;
		}
	}

	return FALSE;
}

Schedule* Scheduler::GetSchedule()
{
	Schedule* schedule = NULL;
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