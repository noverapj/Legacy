#pragma once

#include <map>
#include <list>

class cCommand;

class cSchedule
{
public:
	cSchedule(const uint32 command, const uint32 data, const uint32 size, const uint32 tick) 
		: m_command(command), m_data(data), m_size(size), m_tick(tick), m_last(0)
	{}

	BOOL IsTimeup(uint32 current)
	{
		if(m_tick == 0)
			return TRUE;

		if((m_last == 0) || ((current - m_last) > m_tick))
		{
			m_last = current;
			return TRUE;
		}
		return FALSE;
	}

	uint32 GetCommand()		{ return m_command;	}
	uint32 GetData()		{ return m_data;	}
	uint32 GetSize()		{ return m_size;	}

private:
	uint32 m_command;
	uint32 m_data;
	uint32 m_size;

	uint32 m_tick;
	uint32 m_last;
};

class cScheduler
{
public:
	cScheduler(void);
	~cScheduler(void);

	void Init();
	void Destroy();

public:
	BOOL Begin();
	void End();

	BOOL AddSchedule(const uint32 command, const uint32 data, const uint32 size, const uint32 tick);

	cSchedule* GetSchedule();

private:
	typedef std::list<cSchedule*> SCHEDULES;

	SCHEDULES m_schedules;
	SCHEDULES::iterator m_iterator;

	uint32 m_current;
};

extern cScheduler g_scheduler;
