// cNetwork.h: interface for the cNetwork class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include "cPooler.h"
#include "cConnectorManager.h"
#include "cServerSocketManager.h"

class cConnectorQueueElem;


class cNetwork  
{
public:
	cNetwork();
	virtual ~cNetwork();

	void Init();
	void Destroy();

public:
	BOOL Startup( const uint32 maximum );
	BOOL CreateServerSocket( uint16 port, int32 backlog, uint32& portId );

	BOOL AcceptReqeust( uint32 portId );
	void AcceptCompleted( uint32 portId );

	void Finish( cIocpContext* iocpContext );

public:
	BOOL AddConnector( const uint32 ID, const TCHAR* IP, const uint16 port );
	void SendConnector( cConnectorQueueElem* command );

protected:
	cPooler				 m_pooler;
	cConnectorManager	 m_connectorManager;
	cServerSocketManager m_serverSocketManager;
};

