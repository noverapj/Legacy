// cCommandQueue.cpp: implementation of the cCommandQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "cIocpContext.h"
#include "cCommandQueue.h"
#include "cCommandQueueElem.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCommandQueue::cCommandQueue(const uint32 maxCount)
{
	m_pooler.CreatePool(100, maxCount);
}

cCommandQueue::~cCommandQueue()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cCommandQueue::PushCommand(	const cIocpContext* iocpContext,
									const uint32 command, 
									const uint8* buffer, 
									const uint32 length )
{
	cCommandQueueElem* element = m_pooler.Pop();
	if( !element )
	{
		Debug( _T("LS system :: not enough command queue #1\n") );
		return;
	}

	if( element->CopyData(	iocpContext,
							command,
							buffer,
							length ) )
	{
		Enqueue( reinterpret_cast<DWORD>(element), sizeof(cCommandQueueElem));
	}
}

void cCommandQueue::PushCommand(	const uint32 ID,
									const uint32 command, 
									const uint8* buffer, 
									const uint32 length )
{
	cCommandQueueElem* element = m_pooler.Pop();
	if( !element )
	{
		Debug( _T("LS system :: not enough command queue #2\n") );
		return;
	}

	if( element->CopyData(	ID,
							command,
							buffer,
							length ) )
	{
		Enqueue( reinterpret_cast<DWORD>(element), sizeof(cCommandQueueElem));
	}
}

void cCommandQueue::Free( cCommandQueueElem* element )
{
	m_pooler.Push(element);
}
