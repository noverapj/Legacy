#pragma once

#include "cBaseThread.h"

class cTestThread : public cBaseThread
{
public:
	cTestThread(void);
	~cTestThread(void);

	void Init();
	void Destroy();

protected:
	BOOL InitializeSoap(DWORD& time);

protected:
    virtual void Process();

};

