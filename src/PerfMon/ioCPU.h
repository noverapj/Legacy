#pragma once

#include "ioPDH.h"

class ioCPU : public ioPDH
{
public:
	ioCPU(void);
	~ioCPU(void);

	void Init();
	void Destroy();

protected:
	BOOL Startup();
	void Cleanup();

public:
	BOOL GetCPUs(UNITS& units);
	int GetProcessors()	{ return m_processors; }

protected:
	int m_processors;
};

