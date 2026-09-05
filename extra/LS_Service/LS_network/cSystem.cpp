// cSystem.cpp: implementation of the cSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cSystem.h"
#include "../include/cSingleton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cSystem::cSystem() : m_IOCP(NULL), m_network(NULL), m_module(NULL), m_communicator(NULL), m_packet(0), m_encoding(0), m_processor(NULL)
{
	Init();
}

cSystem::~cSystem()
{
	Destroy();
}

void cSystem::Init()
{
	Trace( _T("cSystem::Init()\n") );
}

void cSystem::Destroy()
{
	Trace( _T("cSystem::Destroy()\n") );

	Cleanup();

	if( m_IOCP )
	{
		delete m_IOCP;
		m_IOCP = NULL;
	}
	if( m_network )
	{
		delete m_network;
		m_network = NULL;
	}
	if( m_module )
	{
		delete m_module;
		m_module = NULL;
	}
	if( m_communicator )
	{
		delete m_communicator;
		m_communicator = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

// startup/cleanup 
BOOL cSystem::Startup( const uint32 maxSession, const uint32 maxPacket, const uint32 maxQueue, const uint32 encoding )
{
	SetPool( maxQueue );
	SetPacket( maxPacket );
	SetEncoding( encoding );

	m_IOCP			= new cIOCP;
	m_network		= new cNetwork;
	m_module		= new cModule;
	m_communicator	= new cCommunicator(maxQueue);

	if( m_IOCP->Create() && m_network->Startup(maxSession) )
	{
		Trace( _T("cSystem::Startup() - success.\n") );
		return TRUE;
	}

	Information( _T("cSystem::Startup() - failed.\n") );
	return FALSE;
}

void cSystem::Cleanup()
{
	Trace( _T("cSystem::Cleanup()\n") );
	if( m_IOCP )
	{
		m_IOCP->Close();
	}
}

// start/end module
BOOL cSystem::StartModule( const E_THREADID threadId, const uint32 count )
{
	Trace( _T("cSystem::StartModule(), ThreadID(%d), Count(%d)\n"), threadId, count );

	if( m_module )
	{
		return m_module->StartModule( threadId, count );
	}
	return FALSE;
}

void cSystem::EndModule()
{
	Trace( _T("cSystem::EndModule()\n") );
	if( m_module )
	{
		m_module->EndModule();
	}
}

// set configuration
void cSystem::SetProcessor( IProcessor* processor )
{
	m_processor = processor;
}

void cSystem::SetEvent( const uint32 connectId, const uint32 disconnectId )
{
	m_eventID[ 0 ] = connectId;
	m_eventID[ 1 ] = disconnectId;
}

void cSystem::SetPool( const uint32 pool)
{
	m_pool = pool;
}

void cSystem::SetPacket( const uint32 packet)
{
	m_packet = packet;
}

void cSystem::SetEncoding( const uint32 encoding)
{
	m_encoding = encoding;
}

BOOL cSystem::Listen( const uint16 port, const uint32 backlog ) 
{
	if( m_network )
	{
		// 辑滚家南 积己
		uint32 ID = 0;
		if( m_network->CreateServerSocket( port, backlog, ID ) )
		{
			// nBackLog父怒 Accept夸没
			if( !m_network->AcceptReqeust( ID ) )
			{
				Information( _T("cSystem::Listen() failed.\r\n") );
				return FALSE;
			}
			//Information( _T("cSystem::Listen() success.\r\n") );
			return TRUE;
		}
	}
	Information( _T("cSystem::Listen() failed - network\r\n") );
	return FALSE;
}

BOOL cSystem::Connect( const uint32 ID, const TCHAR* IP, const uint16 port )
{
	if( m_network )
	{
		Trace( _T("cSystem::UseConnector() success.\n") );
		return m_network->AddConnector( ID, IP, port );
	}
	Information( _T("cSystem::UseConnector() failed.\n") );
	return FALSE;
}
