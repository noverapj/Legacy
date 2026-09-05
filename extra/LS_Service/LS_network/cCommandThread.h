#pragma once

#include "cBaseThread.h"

class cCommandQueue;


class cCommandThread : public cBaseThread
{
public:
	cCommandThread();
	virtual ~cCommandThread();

	void Init();
	void Destroy();

protected:
	virtual void Process();
};
