#pragma once

#include <list>

class cCommand;

class Schedule
{
public:
	Schedule(const uint32 command, const uint32 tick) 
		: m_command(command), m_tick(tick), m_last(0)
	{}

	BOOL IsTimeup(uint32 current)
	{
		if(m_tick == 0)
			return TRUE;

		if(m_last == 0)
		{
			m_last = current;
			return TRUE;
		}

		DWORD elapse = current - m_last;
		if(elapse > m_tick)
		{
			m_last = current - (elapse - m_tick);
			return TRUE;
		}
		return FALSE;
	}

	uint32 GetCommand()		{ return m_command;	}
	void SetTick( const uint32 tick ){ m_tick = tick; }

private:
	uint32 m_command;

	uint32 m_tick;
	uint32 m_last;
};

class Scheduler
{
public:
	Scheduler(void);
	~Scheduler(void);

	void Init();
	void Destroy();

public:
	BOOL Begin();
	void End();

	BOOL AddSchedule(const uint32 command, const uint32 tick);
	BOOL ChangeTickValue( const uint32 command, const uint32 tick );

	Schedule* GetSchedule();

	uint32 GetCount()	{ return m_schedules.size(); }

private:
	typedef std::list<Schedule*> SCHEDULES;

	SCHEDULES m_schedules;
	SCHEDULES::iterator m_iterator;

	uint32 m_current;
};

