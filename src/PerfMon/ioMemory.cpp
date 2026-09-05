#include "stdafx.h"
#include "ioMemory.h"


ioMemory::ioMemory(void)
{
	Init();
}

ioMemory::~ioMemory(void)
{
	Destroy();
}

void ioMemory::Init()
{
	Startup();
}

void ioMemory::Destroy()
{
	Cleanup();
}

BOOL ioMemory::Startup()
{
	ioPDH::Startup();

	BOOL result = AddCounter(CNTR_MEMORY_AVAIL_KB);
	result = AddCounter(CNTR_MEMORY_COMMITTED_PERCENT);
	return result;
}

void ioMemory::Cleanup()
{
	PdhCloseQuery(&m_query);
}

BOOL ioMemory::GetMemory(UNITS& units)
{
	SetError( 0 );

	//// collect
	//if(!CollectQuery())
	//	return GetLastError();

	//// update counters
	//for(int i = 0 ; i < GetCounters() ; i++)
	//{
	//	int cpu = GetValue(m_counters[i]->index);
	//	units.push_back(cpu);
	//}

	MEMORYSTATUSEX memStatus;
	ZeroMemory(&memStatus, sizeof(memStatus));

	memStatus.dwLength = sizeof(memStatus);
	if(GlobalMemoryStatusEx( &memStatus ))
	{
		units.push_back(static_cast<int>(memStatus.dwMemoryLoad));
		units.push_back(static_cast<int>(memStatus.ullTotalPhys / 1024));
		units.push_back(static_cast<int>(memStatus.ullAvailPhys / 1024));
		units.push_back(static_cast<int>(memStatus.ullTotalPageFile / 1024));
		units.push_back(static_cast<int>(memStatus.ullAvailPageFile / 1024));
		units.push_back(static_cast<int>(memStatus.ullTotalVirtual / 1024));
		units.push_back(static_cast<int>(memStatus.ullAvailVirtual / 1024));
		return TRUE;
	}
	else
	{
		m_error = GetLastError();
		return FALSE;
	}
}