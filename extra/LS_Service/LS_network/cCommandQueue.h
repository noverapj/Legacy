#pragma once

#include "cIocpQueue.h"
#include "../include/MemPooler.h"


class cIocpContext;
class cCommandQueueElem;

class cCommandQueue : public cIocpQueue
{
public:
	cCommandQueue(const uint32 maxCount);
	virtual ~cCommandQueue();

public:
	void PushCommand(	const cIocpContext* iocpContext,
						const uint32 command, 
						const uint8* buffer = NULL, 
						const uint32 length = 0 );

	void PushCommand(	const uint32 ID,
						const uint32 command, 
						const uint8* buffer = NULL, 
						const uint32 length = 0 );

	void Free( cCommandQueueElem* element );

protected:
	MemPooler<cCommandQueueElem> m_pooler;
};