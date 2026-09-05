#include "stdafx.h"
#include "ioCPU.h"
#include "ioMemory.h"
#include "ioDisk.h"
#include "ioNetwork.h"
#include "../include/PerfMon.h"


ioCPU		CPU;
ioMemory	MEMORY;
ioDisk		DISK;
ioNetwork	NETWORK;

PerfMon::PerfMon(void)
{
	Init();
}

PerfMon::~PerfMon(void)
{
	Destroy();
}

void PerfMon::Init()
{
}

void PerfMon::Destroy()
{
}

BOOL PerfMon::GetCPU(UNITS& units, DWORD& error)
{
	units.clear();

	if(!CPU.GetCPUs(units))
	{
		error = CPU.GetError();
		return FALSE;
	}
	return TRUE;
}

BOOL PerfMon::GetMemory(UNITS& units, DWORD& error)
{
	units.clear();

	if(!MEMORY.GetMemory(units))
	{
		error = MEMORY.GetError();
		return FALSE;
	}
	return TRUE;
}

BOOL PerfMon::GetDisk(TCHAR* drive, UNITS& units, DWORD& error)
{
	units.clear();

	if(!DISK.GetDisk(drive, units))
	{
		error = DISK.GetError();
		return FALSE;
	}
	return TRUE;

}

BOOL PerfMon::GetNetwork(UNITS& units, DWORD& error)
{
	units.clear();

	if(!NETWORK.GetNetworks(units))
	{
		error = NETWORK.GetError();
		return FALSE;
	}
	return TRUE;
}