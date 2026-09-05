#pragma once

#include "ioPDH.h"

class ioDisk : public ioPDH
{
public:
	ioDisk(void);
	~ioDisk(void);

	void Init();
	void Destroy();

protected:
	BOOL Startup();
	void Cleanup();

protected:
	BOOL GetDisk(TCHAR* drive, int& total, int& avail);

public:
	BOOL GetDisk(TCHAR* drive, UNITS& units);

};

