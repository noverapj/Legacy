#pragma once

#include "cConnector.h"
#include <map>

class cConnectorQueueElem;

class cConnectorManager  
{
public:
	cConnectorManager();
	virtual ~cConnectorManager();

	void Init();
	void Destroy();

public:
	void Execute( cConnectorQueueElem* element );

	BOOL AddConnector(	const uint32 ID, 
						const TCHAR* IP, 
						const uint16 port );
	BOOL DelConnector(	const uint32 ID );

protected:
	cConnector*	Get( const uint32 ID );
	cConnector*	Remove( const uint32 ID );

protected:
	typedef std::map<uint32,cConnector*> CONNECTORS;
	CONNECTORS	m_connectors;
};
