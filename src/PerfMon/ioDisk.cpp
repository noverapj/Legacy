#include "stdafx.h"
#include "ioDisk.h"

ioDisk::ioDisk(void)
{
	Init();
}

ioDisk::~ioDisk(void)
{
	Destroy();
}

void ioDisk::Init()
{
	Startup();
}

void ioDisk::Destroy()
{
	Cleanup();
}

BOOL ioDisk::Startup()
{
	ioPDH::Startup();

	BOOL result = AddCounter(CNTR_DISK_FREESPACE);
	result = AddCounter(CNTR_DISK_IDLETIME);
	result = AddCounter(CNTR_DISK_READTIME);
	result = AddCounter(CNTR_DISK_WRITETIME);
	result = AddCounter(CNTR_DISK_TIME);
	
	return result;
}

void ioDisk::Cleanup()
{
	PdhCloseQuery(&m_query);
}

BOOL ioDisk::GetDisk(TCHAR* drive, int& total, int& avail)
{
	DWORD sectorsPerCluster, bytesPerSector, numberOfFreeClusters, totalNumberOfClusters;

	BOOL result = GetDiskFreeSpace(	drive, 
		&sectorsPerCluster,
		&bytesPerSector, 
		&numberOfFreeClusters, 
		&totalNumberOfClusters );
	if(result == FALSE)
	{
		SetError( GetLastError() );
		return FALSE;
	}

	total = sectorsPerCluster * bytesPerSector * (totalNumberOfClusters / 1024);
	avail = sectorsPerCluster * bytesPerSector * (numberOfFreeClusters / 1024);
	return TRUE;
}

BOOL ioDisk::GetDisk(TCHAR* drive, UNITS& units)
{
	SetError( 0 );

	//// collect
	//if(!CollectQuery())
	//	return FALSE;

	//// update counters
	//for(int i = 0 ; i < GetCounters() ; i++)
	//{
	//	int cpu = GetValue(m_counters[i]->index);
	//	units.push_back(cpu);
	//}

	int total = 0, avail = 0;
	if(GetDisk(drive, total, avail))
	{
		units.push_back(avail);
		units.push_back(total);
		return TRUE;
	}
	else
	{
		units.push_back(0);
		units.push_back(0);
	}
	return TRUE;
}