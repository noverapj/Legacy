// cConnectorManager.cpp: implementation of the cConnectorManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cConnectorQueueElem.h"
#include "cConnectorManager.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnectorManager::cConnectorManager()
{
	Init();
}

cConnectorManager::~cConnectorManager()
{
	Destroy();
}


void cConnectorManager::Init()
{
}

void cConnectorManager::Destroy()
{
	cConnector* connector = NULL;
	for( CONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it )
	{
		connector = it->second;
		delete connector;
	}

	m_connectors.clear();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void cConnectorManager::Execute( cConnectorQueueElem* element )
{
	cConnector* connector = Get( element->GetID() );
	if( connector )
	{
		connector->SendPacket(	element->GetCommand(), 
								element->GetData(), 
								element->GetLength() );
	}
}

BOOL cConnectorManager::AddConnector(	
										const uint32 ID, 
										const TCHAR* IP, 
										const uint16 port )
{
	cConnector* connector = Get( ID );
	if( connector )	return FALSE;

	connector = new cConnector;
	if( connector )
	{
		connector->SetIdentity( ID );

		if( !connector->Startup() )		
		{
			delete connector;
			return FALSE;
		}
		if( !connector->Create() )
		{
			delete connector;
			return FALSE;
		}
		if( !connector->Connect( IP, port ) )
		{
			delete connector;
			return FALSE;
		}
		if( !connector->StartThread() )
		{
			delete connector;
			return FALSE;
		}
		m_connectors[ID] = connector;
		return TRUE;
	}

	return FALSE;
}

BOOL cConnectorManager::DelConnector( const uint32 ID )
{
	cConnector* connector = Remove( ID );
	if( connector )
	{
		delete connector;
		return TRUE;
	}
	return FALSE;
}

cConnector*	cConnectorManager::Get( const uint32 ID )
{
	CONNECTORS::iterator it = m_connectors.find( ID );
	return ( it != m_connectors.end() ) ? it->second : NULL;
}

cConnector*	cConnectorManager::Remove( const uint32 ID )
{
	CONNECTORS::iterator it = m_connectors.find( ID );
	if( it != m_connectors.end() )
	{
		cConnector* connector = it->second;
		m_connectors.erase( it );
		return connector;
	}
	return NULL;
}

