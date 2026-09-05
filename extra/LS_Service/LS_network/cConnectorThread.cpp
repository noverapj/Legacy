// cConnectorThread.cpp: implementation of the cConnectorThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cConnectorQueue.h"
#include "cConnectorQueueElem.h"
#include "cConnectorThread.h"
#include "cConnectorManager.h"
#include "cSystem.h"
#include "../include/cSingleton.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnectorThread::cConnectorThread()
{
	Init();
}

cConnectorThread::~cConnectorThread()
{
	Destroy();
}

void cConnectorThread::Init()
{
}

void cConnectorThread::Destroy()
{
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cConnectorThread::Process()
{
	cConnectorQueue* connectorQueue = cSingleton<cSystem>::GetInstance()->GetConnectorQueue();
	if( !connectorQueue ) return;

	Trace( _T("ConnectorThread :: %lu\n"), GetCurrentThreadId() );

	cNetwork* network = cSingleton<cSystem>::GetInstance()->GetNetwork();

	cConnectorQueueElem* command = NULL;
	while( !IsTerminated() )
	{
		command = reinterpret_cast<cConnectorQueueElem*>(connectorQueue->Dequeue());
		if( command )
		{
			// process command
			network->SendConnector( command );

			// delete a command
			connectorQueue->Free( command );
			
		}
	}
}
