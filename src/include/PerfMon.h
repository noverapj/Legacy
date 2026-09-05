#pragma once

#include <vector>

typedef std::vector<int> UNITS;

class PerfMon
{
public:
	PerfMon(void);
	~PerfMon(void);

	void Init();
	void Destroy();

public:
	BOOL GetCPU(UNITS& units, DWORD& error);
	BOOL GetMemory(UNITS& units, DWORD& error);
	BOOL GetDisk(TCHAR* drive, UNITS& units, DWORD& error);
	BOOL GetNetwork(UNITS& units, DWORD& error);
};

