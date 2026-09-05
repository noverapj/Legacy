#include "stdafx.h"

#include "CopyNodeParent.h"
#include "ServerNode.h"

CopyNodeParent::CopyNodeParent() : m_pCreator(NULL), m_eCopyType( NONE_TYPE )
{
}

CopyNodeParent::~CopyNodeParent()
{	

}

void CopyNodeParent::OnCreate( ServerNode *pCreator )
{
	m_pCreator = pCreator;
}

void CopyNodeParent::OnDestroy()
{
	m_pCreator = NULL;
}

const ioHashString &CopyNodeParent::GetServerIP()
{ 
	static ioHashString szReturn = "ERROR 1";
	if( m_pCreator == NULL )
		return szReturn;
	return m_pCreator->GetServerIP(); 
}

const int   &CopyNodeParent::GetServerPort()
{ 
	static int iReturn = -1;
	if( m_pCreator == NULL )
		return iReturn;
	return m_pCreator->GetServerPort(); 
}

const int   &CopyNodeParent::GetClientPort()
{ 
	static int iReturn = -1;
	if( m_pCreator == NULL )
		return iReturn;
	return m_pCreator->GetClientPort(); 
}

bool CopyNodeParent::SendMessage( SP2Packet &rkPacket )
{
	if( m_pCreator == NULL )
		return false;
	return m_pCreator->SendMessage( rkPacket );
}