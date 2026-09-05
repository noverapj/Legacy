#include "StdAfx.h"
#include "ServerInfoManager.h"
#include "ServerConnectorMgr.h"


ServerConnectorMgr::ServerConnectorMgr(void) : m_partitions(1)
{
	Init();
 }

ServerConnectorMgr::~ServerConnectorMgr(void)
{
	Destroy();
}

void ServerConnectorMgr::Init()
{
	m_connectors.reserve(g_Config()->NGameServerMax());
}

void ServerConnectorMgr::Destroy()
{
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		SVRCONNECTINFO_ serverInfo;
		ZeroMemory(&serverInfo, sizeof(serverInfo));

		g_ConnectAssist()->PutConnectData(ECONNECTASSIST::CLOSE, serverInfo, *it);
	}
	m_connectors.clear();
}

void ServerConnectorMgr::Connect(VIPADDR& vIpAddr)  
{
	for(VIPADDR::iterator it = vIpAddr.begin() ; it != vIpAddr.end() ; ++it)  
	{
		SVRCONNECTINFO_ serverInfo = *it;
		g_ConnectAssist()->PutConnectData(ECONNECTASSIST::CONNECT, serverInfo);
	}
}

void ServerConnectorMgr::AddConnector(LSConnector* node)
{ 
	m_connectors.push_back(node);
}

void ServerConnectorMgr::DelConnector(LSConnector* node)
{ 
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		if(connector == node)
		{
			SVRCONNECTINFO_ addr;
			addr.port			= connector->m_port;
			addr.serverIndex	= connector->m_sendServerId;
			strcpy_s(addr.ipAddr, connector->m_ipAddr.c_str());
		
			g_ConnectAssist()->PutConnectData(ECONNECTASSIST::CLOSE, addr, node);
			g_ServerInfoMgr()->DelServerInfo(node->GetSocket());

			m_connectors.erase( it );
			break;
		}
	}
}
 
void ServerConnectorMgr::Broadcast(SP2Packet& sp)
{
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		if(connector)
		{
			connector->SendMessage(sp);
		}
	}
}

void ServerConnectorMgr::RequestStatus()
{
	SP2Packet pk(EPROTOCOL::LSTPK_STATUS_REQUEST);
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		if(!connector) continue;

		connector->SendMessage(pk);
		connector->IncreaseReferenceCount();

		if(connector->GetReferenceCount() > 3)
		{
			connector->UpdateState(ESVRSTATE::LS_ZOMBIESERVERSTATE);
		}
	}
}

void ServerConnectorMgr::RequestPartition()
{
	int partitionCount = 0;
	if(!RearrangePartition(partitionCount)) return;

	// 총인원을 토대로 채널분리 여부를 결정한다
	SP2Packet packet(EPROTOCOL::LSPTK_PARTITION_REQUEST);
	packet << partitionCount;

	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		if(!connector) continue;

		connector->SendMessage(packet);
	}
}

void ServerConnectorMgr::Flush()
{
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		if(!connector->IsActive() || (connector->GetSocket() == INVALID_SOCKET))
			continue;

		connector->FlushSendBuffer();
	}
}

void ServerConnectorMgr::Print()
{
	for(LSCONNECTORS::iterator it = m_connectors.begin() ; it != m_connectors.end() ; ++it)  
	{
		LSConnector* connector = *it;
		Debug(_T("%s:%d::S:%d\r\n"), connector->m_ipAddr.c_str(), connector->m_port, connector->GetReferenceCount());
	}
}

BOOL ServerConnectorMgr::RearrangePartition(int& partitionCount)
{
	// 접속한 총인원
	int userCount		= g_ServerInfoMgr()->GetUserCount();
	int partitionUser	= g_Config()->GetPartition();
	if( 0 == partitionUser ) return FALSE;

	partitionCount = userCount / partitionUser;
	if((userCount % partitionUser) != 0)
	{
		++partitionCount;
	}

	partitionCount = (partitionCount < 1) ? 1 : partitionCount;
	if(partitionCount != m_partitions)
	{
		// 파티션 변경되어야 함
		m_partitions = partitionCount;
		return TRUE;
	}

	return FALSE;
}