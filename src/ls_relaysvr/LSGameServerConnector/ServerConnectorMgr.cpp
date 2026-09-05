#include "StdAfx.h"
#include "ServerConnectorMgr.h"


ServerConnectorMgr::ServerConnectorMgr(void) : m_timerState(false)
{
}

ServerConnectorMgr::~ServerConnectorMgr(void)
{
	ReleaseMemoryPool();
}

void ServerConnectorMgr::Init( )
{
	g_ConnectAssist()->Init();
	g_ConnectAssist()->InitMemoryPool();
	g_ConnectAssist()->Begin();
	 
}

void ServerConnectorMgr::InitMemoryPool()
{
	int max = g_Config()->GetGameServerMaxSize();
	m_serverNodes.reserve(max);
}

void ServerConnectorMgr::ReleaseMemoryPool()
{
	//컨넥트 어시스트에 putq
	for(int i=0; i<(int)m_serverNodes.size(); ++i)
	{
		SVRCONNECTINFO_ addr;
		g_ConnectAssist()->PutConnectData(ConnectAssistTypes::CLOSE,addr,m_serverNodes[i]);
	}
	m_serverNodes.clear();
}

void ServerConnectorMgr::PushClient(GameServerNode* node)
{
	OnAccept_ stAccept;

	stAccept.opid = OperationIndex::ONACCEPT;
	stAccept.node = node;

	PUTQFUNNC(stAccept,(g_Queue()));
}

void ServerConnectorMgr::AddClient(GameServerNode* node)
{ 
	m_serverNodes.push_back(node);
}

bool ServerConnectorMgr::DelClient(GameServerNode* node)
{ 
	for(int i=0; i<(int)m_serverNodes.size();++i)
	{
		if(m_serverNodes[i] == node)
		{
			SVRCONNECTINFO_ addr;
			GameServerNode* serverNode = m_serverNodes[i];
			strcpy_s(addr.ipAddr,serverNode->ServerAddress().c_str());
			addr.port = serverNode->ServerPort();
			addr.serverIndex = serverNode->SendServerId();

			m_serverNodes.erase(m_serverNodes.begin()+i);

			g_ConnectAssist()->PutConnectData(ConnectAssistTypes::CLOSE, addr, node);
			break;
		}
	}
	return true;
}

void ServerConnectorMgr::CreateConnectoClients(ConnectIPAddrs& IpAddrs)  
{
	for(int i=0; i<(int)IpAddrs.size(); ++i)  
	{
		SVRCONNECTINFO_& addr = IpAddrs[i];
		ConnectClient(addr);
	}
}

void ServerConnectorMgr::ConnectClient(SVRCONNECTINFO_& connectAddr)
{  
	g_ConnectAssist()->PutConnectData(ConnectAssistTypes::CONNECT,connectAddr);
}

void ServerConnectorMgr::SendMessageAllNode(SP2Packet& sPacket)
{
	for(int i=0; i<(int)m_serverNodes.size(); ++i)
	{
		GameServerNode* serverNode = m_serverNodes[i];
		serverNode->SendMessage(sPacket);
	}
}

void ServerConnectorMgr::SendBufferFlush()
{
	for(int i=0; i<(int)m_serverNodes.size(); ++i)
	{
		GameServerNode* serverNode = m_serverNodes[i];
		if(!serverNode->IsActive() || serverNode->GetSocket() == INVALID_SOCKET)
			continue;
	//	LOG.PrintTimeAndLog(0,"SendBufferFlush Success");
		serverNode->FlushSendBuffer();
	}
}

GameServerNode* ServerConnectorMgr::GetServerNodeByID( int serverID )
{
	for(int i=0; i<(int)m_serverNodes.size(); ++i)
	{
		GameServerNode* serverNode = m_serverNodes[i];
		if(serverNode->IsActive() && serverNode->GetSocket() != INVALID_SOCKET)
		{
			if(serverNode->ServerIndex() == serverID)
				return serverNode;
		}
	}
	return NULL;
}

int ServerConnectorMgr::GetRoomCount()
{
	int sum = 0;
	for(UINT i=0; i<m_serverNodes.size(); ++i)
	{
		GameServerNode* serverNode = m_serverNodes[i];
		sum += serverNode->GetRoomCount();
	}
	return sum;
}

