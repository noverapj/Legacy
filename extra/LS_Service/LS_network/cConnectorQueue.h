#pragma once

#include "cIocpQueue.h"
#include "../include/MemPooler.h"

class cConnectorQueueElem;

class cConnectorQueue : public cIocpQueue 
{
public:
	cConnectorQueue();
	virtual ~cConnectorQueue();

public:
	void PushCommand(	const uint32 ID,
						const uint32 command, 
						const uint8* buffer = NULL, 
						const uint16 length = 0 );

	void Free( cConnectorQueueElem* element );

protected:
	MemPooler<cConnectorQueueElem> m_pooler;
};