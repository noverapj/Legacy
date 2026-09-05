// cConnectorQueue.cpp: implementation of the cConnectorQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSystem.h"
#include "cConnectorQueue.h"
#include "cConnectorQueueElem.h"
#include "../include/cSingleton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnectorQueue::cConnectorQueue()
{
	m_pooler.CreatePool(100, cSingleton<cSystem>::GetInstance()->GetPool());
}

cConnectorQueue::~cConnectorQueue()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cConnectorQueue::PushCommand(	const uint32 ID,
									const uint32 command, 
									const uint8* buffer,
									const uint16 length )
{
	cConnectorQueueElem* element = m_pooler.Pop();
	if( element )
	{
		if( element->CopyData(	ID,
								command,
								buffer,
								length ) )
		{
			Enqueue( reinterpret_cast<DWORD>(element), sizeof(cConnectorQueueElem) );
		}
	}
}


void cConnectorQueue::Free( cConnectorQueueElem* element )
{
	m_pooler.Push(element);
}
