#include "StdAfx.h"
#include "MonitorsManager.h"


MonitorsManager::MonitorsManager(void)
{
}

MonitorsManager::~MonitorsManager(void)
{
	ReleaseMemoryPool();
}

void MonitorsManager::AddClient(MonitorNode* node)
{
	g_State()->IncrementAccept();
	if(g_State()->GetAcceptNowCount() == 0) //kyg ¹Ù²Ü°Í 
		g_State()->SetAcceptNowCount(0);
	m_nodes.AddTail(node);
}

bool MonitorsManager::DelClient(MonitorNode* node)
{
	POSITION pos = m_nodes.Find(node);

	if(pos == NULL)
	{
		LOG.PrintTimeAndLog(0,"DelClient fail\n");
		return false;
	}
	
	m_nodes.RemoveAt(pos);
	m_memPool.Push(node);
	g_State()->IncrementClose();
	return true;
}

bool MonitorsManager::CreateClientNode(SOCKET s)
{
	MonitorNode *node = (MonitorNode*)m_memPool.Pop();

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

void MonitorsManager::InitMemoryPool()
{
	int nSend = g_Config()->GetConnectMgrSendSize();
	int nRead = g_Config()->GetConnectMgrRecvSize();
	int nMax  = g_Config()->GetConnectMgrMaxPoolSize();
	m_memPool.CreatePool(nMax/2,nMax);
	//m_memPool.CreatePool(0,nMax);
	//for(int i=0; i< nMax; ++i)
		//m_memPool.CreatePush(new MonitorNode(INVALID_SOCKET,nSend,nRead)); //kyg º¯È¯ 
}

void MonitorsManager::ReleaseMemoryPool()
{
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		MonitorNode* node = m_nodes.GetAt(pos);
		node->OnDestroy();
		m_memPool.Push(node);
		m_nodes.GetNext(pos);
	}
	m_nodes.RemoveAll();
	m_memPool.DestroyPool();
}

void MonitorsManager::BraodcastFilter( const int connectType, SP2Packet &rkPacket )
{
	__try
	{
		POSITION pos = m_nodes.GetHeadPosition();
		while(pos)
		{
			MonitorNode* node = m_nodes.GetAt(pos);
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

void MonitorsManager::Broadcast( SP2Packet &rkPacket )
{
	__try
	{
		POSITION pos = m_nodes.GetHeadPosition();
		while(pos)
		{
			MonitorNode* node = m_nodes.GetAt(pos);
			node->SendMessage(rkPacket);			 
			m_nodes.GetNext(pos);
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}

}


void MonitorsManager::SendBufferFlush()
{
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		MonitorNode* node = m_nodes.GetAt(pos);
		if(!node->IsActive() || node->GetSocket() == INVALID_SOCKET)
		{
			m_nodes.GetNext(pos);
			continue;
		}
		node->FlushSendBuffer();
		m_nodes.GetNext(pos);
	}
}

bool MonitorsManager::SendCloseMessage(int fd)
{
	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		MonitorNode* node = m_nodes.GetAt(pos);
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

bool MonitorsManager::SendMessageNode(int fd,SP2Packet &rkPacket)
{

	POSITION pos = m_nodes.GetHeadPosition();
	while(pos)
	{
		MonitorNode* node = m_nodes.GetAt(pos);
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

 