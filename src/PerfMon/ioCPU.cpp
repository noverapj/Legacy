#include "stdafx.h"
#include "ioCPU.h"


ioCPU::ioCPU(void) : m_processors(0)
{
	Init();
}

ioCPU::~ioCPU(void)
{
	Destroy();
}

void ioCPU::Init()
{
	Startup();
}

void ioCPU::Destroy()
{
	Cleanup();
}

BOOL ioCPU::Startup()
{
	ioPDH::Startup();

	SYSTEM_INFO sysInfo;
	GetSystemInfo( &sysInfo );

	m_processors = sysInfo.dwNumberOfProcessors;

	BOOL result = AddCounter(CNTR_CPU);
	if(m_processors > 1)
	{
		TCHAR counter[512];
		for(int i = 0 ; i < m_processors ; i++)
		{
			_stprintf(counter, _T("\\Processor(%d)\\%% Processor Time"), i);
			result = AddCounter(counter);
		}
	}
	return result;
}

void ioCPU::Cleanup()
{
	PdhCloseQuery(&m_query);
}

BOOL ioCPU::GetCPUs(UNITS& units)
{
	SetError( 0 );

	// collect
	if(!CollectQuery())
		return FALSE;

	// update counters
	for(int i = 0 ; i < GetCounters() ; i++)
	{
		int cpu = GetValue(m_counters[i]->index);
		units.push_back(cpu);
	}
	return TRUE;
}

