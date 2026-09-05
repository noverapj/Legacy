// cConnectorQueueElem.cpp: implementation of the cConnectorQueueElem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cConnectorQueueElem.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnectorQueueElem::cConnectorQueueElem()
{
	Init();
}

cConnectorQueueElem::~cConnectorQueueElem()
{
	Destroy();
}


void cConnectorQueueElem::Init()
{
	m_ID = 0;
	m_command = 0;
}

void cConnectorQueueElem::Destroy()
{
	m_ID = 0;
	m_command = 0;
}


//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////
	
BOOL cConnectorQueueElem::CopyData(	const uint32 ID,
									const uint32 command,
									const uint8* buffer, 
									const uint32 length )
{
	if( cBaseQueueElem::CopyData( buffer, length ) )
	{
		m_ID = ID;
		m_command = command;
		return TRUE;
	}
	return FALSE;
}
