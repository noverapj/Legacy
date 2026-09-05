#pragma once

#include "cBaseThread.h"


class cConnectorThread : public cBaseThread
{
public:
	cConnectorThread();
	virtual ~cConnectorThread();

	void Init();
	void Destroy();

protected:
	virtual void Process();
};