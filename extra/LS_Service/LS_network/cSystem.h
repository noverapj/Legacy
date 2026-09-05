// cSystem.h: interface for the cSystem class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "cIOCP.h"
#include "cNetwork.h"
#include "cModule.h"
#include "cCommunicator.h"
#include "cCommandQueue.h"
#include "../include/IProcessor.h"


class cSystem  
{
public:
	cSystem();
	virtual ~cSystem();

	void Init();
	void Destroy();

public:
	BOOL Startup( const uint32 maxSession, const uint32 maxPacket, const uint32 maxQueue, const uint32 encoding);
	void Cleanup();

	BOOL StartModule( const E_THREADID threadId, const uint32 count );
	void EndModule();

	void SetProcessor( IProcessor* processor );	
	void SetEvent( const uint32 connectId, const uint32 disconnectId );
	inline void SetPool( const uint32 pool);
	inline void SetPacket( const uint32 packet);
	inline void SetEncoding( const uint32 encoding);

	BOOL Listen( const uint16 port, const uint32 backlog );
	BOOL Connect( const uint32 ID, const TCHAR* IP, const uint16 port );

public:
	IProcessor* GetProcessor()
	{
		return ( m_processor ? m_processor : NULL );
	}

	cCommandQueue* GetCommandQueue()
	{	
		return ( m_communicator ? m_communicator->GetCommandQueue() : NULL );
	}

	cConnectorQueue* GetConnectorQueue()
	{	
		return ( m_communicator ? m_communicator->GetConnectorQueue() : NULL );
	}

	uint32 GetConnectEvent()			{ return m_eventID[0];		}
	uint32 GetDisconnectEvent()			{ return m_eventID[1];		}
	uint32 GetPool()					{ return m_pool;			}
	uint32 GetPacket()					{ return m_packet;			}
	uint32 GetEncoding()				{ return m_encoding;		}

public:
	cIOCP*			GetIOCP()			{ return m_IOCP;			}
	cNetwork*		GetNetwork()		{ return m_network;			}
	cModule*		GetModule()			{ return m_module;			}			
	cCommunicator*	GetCommunicator() 	{ return m_communicator;	}

private:
	cIOCP*			m_IOCP;
	cNetwork*		m_network;
	cModule*		m_module;
	cCommunicator*	m_communicator;

private:
	uint32			m_eventID[2];
	uint32			m_pool;
	uint32			m_packet;
	uint32			m_encoding;
	IProcessor*		m_processor;
};


