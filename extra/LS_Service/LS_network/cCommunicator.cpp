// cCommunicator.cpp: implementation of the cCommunicator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cCommandQueue.h"
#include "cConnectorQueue.h"
#include "cCommunicator.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCommunicator::cCommunicator(const uint32 maxCount)
{
	Init(maxCount);
}

cCommunicator::~cCommunicator()
{
	Destroy();
}

void cCommunicator::Init(const uint32 maxCount)
{
	CreateCommandQueue(maxCount);
	CreateConnectorQueue();
}

void cCommunicator::Destroy()
{
	DestroyCommandQueue();
	DestroyConnectorQueue();
}


//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

BOOL cCommunicator::CreateCommandQueue(const uint32 maxCount, const uint32 timeout)
{
	m_commandQueue = new cCommandQueue(maxCount);
	if( m_commandQueue )
	{
		m_commandQueue->Startup(timeout);
		return TRUE;
	}
	return FALSE;
}

BOOL cCommunicator::CreateConnectorQueue()
{
	m_connectorQueue = new cConnectorQueue;
	if( m_connectorQueue )
	{
		m_connectorQueue->Startup();
		return TRUE;
	}
	return FALSE;
}

void cCommunicator::DestroyCommandQueue()
{
	if( m_commandQueue )
	{
		delete m_commandQueue;
		m_commandQueue = NULL;
	}
}

void cCommunicator::DestroyConnectorQueue()
{
	if( m_connectorQueue )
	{
		delete m_connectorQueue;
		m_connectorQueue = NULL;
	}
}

