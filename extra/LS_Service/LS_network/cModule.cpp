// cModule.cpp: implementation of the cModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cWorkerThread.h"
#include "cCommandThread.h"
#include "cConnectorThread.h"

#include "cModule.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cModule::cModule()
{
	Init();
}

cModule::~cModule()
{
	Destroy();
}

void cModule::Init()
{
	m_workerThread		= NULL;
	m_commandThread		= NULL;
	m_connectorThread	= NULL;
}

void cModule::Destroy()
{
	EndModule();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

// thread module을 시작한다
BOOL cModule::StartModule( E_THREADID threadId, uint32 count )
{
	switch( threadId )
	{
	case E_THREADID_COMMAND :
		StartCommandThread( count );
		break;

	case E_THREADID_CONNECTOR :
		StartConnectorThread( count );
		break;

	case E_THREADID_WORKER :
		StartWorkerThread( count );
		break;
	}

	return TRUE;
}

// thread module을 종료한다
void cModule::EndModule()
{
	CloseWorkerThread();
	CloseCommandThread();
	CloseConnectorThread();
}

//////////////////////////////////////////////////////////////////////
// thread module 
//////////////////////////////////////////////////////////////////////

BOOL cModule::StartCommandThread( uint32 count )
{
	CloseCommandThread();

	m_commandThread = new cCommandThread[count];
	if( m_commandThread )
	{
		for( uint32 i = 0 ; i < count; i++ )
		{
			if( !m_commandThread[i].StartThread() )	return FALSE;
		}
		return TRUE;
	}

	return FALSE;
}	

BOOL cModule::StartWorkerThread( uint32 count )
{
	CloseWorkerThread();

	m_workerThread	= new cWorkerThread[count];
	if( m_workerThread )
	{
		for( uint32 i = 0 ; i < count; i++ )
		{
			if( !m_workerThread[i].StartThread() )	return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL cModule::StartConnectorThread( uint32 count )
{
	CloseConnectorThread();

	m_connectorThread = new cConnectorThread[count];
	if( m_connectorThread )
	{
		for( uint32 i = 0 ; i < count; i++ )
		{
			if( !m_connectorThread[i].StartThread() )	return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

void cModule::CloseCommandThread()
{
	if( m_commandThread )
	{
		delete []m_commandThread;
		m_commandThread = NULL;
	}
}

void cModule::CloseWorkerThread()
{
	if( m_workerThread )
	{
		delete []m_workerThread;
		m_workerThread = NULL;
	}
}

void cModule::CloseConnectorThread()
{
	if( m_connectorThread )
	{
		delete []m_connectorThread;
		m_connectorThread = NULL;
	}
}
