#pragma once

#include "ioPDH.h"

class ioMemory : public ioPDH
{
public:
	ioMemory(void);
	~ioMemory(void);

	void Init();
	void Destroy();

protected:
	BOOL Startup();
	void Cleanup();

public:
	BOOL GetMemory(UNITS& units);
};

