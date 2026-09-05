#include "StdAfx.h"
#include "ClientsManager.h"


ClientsManager::ClientsManager(void)
{
}


ClientsManager::~ClientsManager(void)
{
	ReleaseMemoryPool();
}
void ClientsManager::AddClient(ClientNode* node)
{
	g_State()->IncrementAcceptCount();
	if(g_State()->CurrentAcceptCount() == 0) //kyg ¹Ù²Ü°Í 
		g_State()->CurrentAcceptCount(0);
	m_nodes.AddTail(node);
}

bool ClientsManager::DelClient(ClientNode* node)
{
	POSITION pos = m_nodes.Find(node);
	if(pos == NULL)
	{
		LOG.PrintTimeAndLog(0,"DelClient fail\n");
		return false;
	}
	
	m_nodes.RemoveAt(pos);
	m_memPool.Push(node);
	g_State()->IncrementCloseCount();
	return true;
}
bool ClientsManager::CreateClientNode(SOCKET s)
{
	ClientNode *node = (ClientNode*)m_memPool.Pop();
	if(!node)
	{
		LOG.PrintTimeAndLog(0,"MgrToolNodeManager::CreateMgrToolNode MemPool Zero!");
		return false;
	}
	g_Iocp()->AddHandleToIOCP((HANDLE)s,(DWORD)node);
	node->SetSocket(s);
	node->OnCreate();
	AddClient(node);
	if(!node->AfterCreate())
	{
		node->SessionClose();
		return false;
	}
	return true;
}

void ClientsManager::InitMemoryPool()
{
	int SendCount = g_Config()->NCMgrSendSize();
	int nRead = g_Config()->NCMgrRecvSize();
	int nMax  = g_Config()->NCMgrMaxPool();
	m_memPool.CreatePool(nMax/2,nMax);
	//m_memPool.CreatePool(0,nMax);
	//for(int i=0; i< nMax; ++i)
		//m_memPool.CreatePush(new ClientNode(INVALID_SOCKET,SendCount,nRead)); //kyg º¯È¯ 
}

void ClientsManager::ReleaseMemoryPool()
{
	auto pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		ClientNode* node = m_nodes.GetAt(pos);
		node->OnDestroy();
		m_memPool.Push(node);
		m_nodes.GetNext(pos);
	}
	m_nodes.RemoveAll();
	m_memPool.DestroyPool();
}

void ClientsManager::BraodcastFilter( const int connectType, SP2Packet &rkPacket )
{
	__try
	{
		auto pos = m_nodes.GetHeadPosition();
		while(pos)
		{
			ClientNode* node = m_nodes.GetAt(pos);
			if(node->GetConnectType() == connectType)
			{
				node->SendMessage(rkPacket);
				
			}
			m_nodes.GetNext(pos);
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}
}

void ClientsManager::Broadcast( SP2Packet &rkPacket )
{
	__try
	{
		auto pos = m_nodes.GetHeadPosition();
		while(pos)
		{
			ClientNode* node = m_nodes.GetAt(pos);
			node->SendMessage(rkPacket);			 
			m_nodes.GetNext(pos);
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}

}

void ClientsManager::Flush()
{
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		ClientNode* node = m_nodes.GetAt(pos);
		if(!node->IsActive() || node->GetSocket() == INVALID_SOCKET)
		{
			m_nodes.GetNext(pos);
			continue;
		}
		node->FlushSendBuffer();
		m_nodes.GetNext(pos);
	}
}

bool ClientsManager::SendCloseMessage(int fd)
{
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		ClientNode* node = m_nodes.GetAt(pos);
		if(!node->IsActive() || node->GetSocket() == INVALID_SOCKET)
		{
			m_nodes.GetNext(pos);
			continue;
		}
		if(node->GetSocket() == fd)
		{
			node->SessionClose();
		}

		m_nodes.GetNext(pos);
	}
	return true;
}
bool ClientsManager::SendMessageNode(int fd,SP2Packet &rkPacket)
{

	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		ClientNode* node = m_nodes.GetAt(pos);
		if(!node->IsActive() || node->GetSocket() == INVALID_SOCKET)
		{
			m_nodes.GetNext(pos);
			continue;
		}
		if(node->GetSocket() == fd)
		{
			node->SendMessage(rkPacket);
			break;
		}
	 
		m_nodes.GetNext(pos);
	}

	return true; 
}

void ClientsManager::DelGostClient()
{
#if 1
	int tnow = std::clock();
	if(m_nodes.GetCount() == 0)
		return;
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		ClientNode* node = m_nodes.GetAt(pos);
		if(!node->IsActive() || node->GetSocket() == INVALID_SOCKET || node->GetConnectType() != ENODETYPE::USER)
		{
			m_nodes.GetNext(pos);
			continue;
		}
		if(node->IsGhost() != ECONNECTSTATE::LS_USERCONNECTED)
		{
			if((tnow - node->Currenttime()) >= g_Config()->ClientGhostTime())
			{
				node->SessionClose();
				LOG.PrintTimeAndLog(0,"Ghost Client :%s(ecdoe:%d)",node->GetPrivateIP(),node->IsGhost());
			}
		}
		m_nodes.GetNext(pos);
	}
#endif
}
