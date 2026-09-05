// cCommandQueueElemElem.cpp: implementation of the cCommandQueueElemElem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cIocpContext.h"
#include "cCommandQueueElem.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCommandQueueElem::cCommandQueueElem()
{
	Init();
}

cCommandQueueElem::~cCommandQueueElem()
{
	Destroy();
}

void cCommandQueueElem::Init()
{
	m_ID		= 0;
	m_command	= 0;
	m_context	= NULL;
}

void cCommandQueueElem::Destroy()
{
	m_ID		= 0;
	m_command	= 0;
	m_context	= NULL;
}


//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////
	
BOOL cCommandQueueElem::CopyData(	const cIocpContext* iocpContext, 
									const uint32 command,
									const uint8* buffer, 
									const uint32 length )
{
	if( cBaseQueueElem::CopyData( buffer, length ) )
	{
		m_context	= const_cast<cIocpContext*>(iocpContext);
		m_ID		= 0;
		m_command	= command;
		return TRUE;
	}
	return FALSE;
}

BOOL cCommandQueueElem::CopyData(	const uint32 ID, 
									const uint32 command,
									const uint8* buffer, 
									const uint32 length )
{
	if( cBaseQueueElem::CopyData( buffer, length ) )
	{
		m_context	= NULL;
		m_ID		= ID;
		m_command	= command;
		return TRUE;
	}
	return FALSE;
}

