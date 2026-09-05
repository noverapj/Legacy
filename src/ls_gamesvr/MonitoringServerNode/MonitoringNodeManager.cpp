#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Network/iocpHandler.h"
#include ".\monitoringnodemanager.h"

MonitoringNodeManager *MonitoringNodeManager::sg_Instance = NULL;

MonitoringNodeManager::MonitoringNodeManager(void)
{
}

MonitoringNodeManager::~MonitoringNodeManager(void)
{
	m_vNode.clear();	
}

MonitoringNodeManager &MonitoringNodeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new MonitoringNodeManager;

	return *sg_Instance;
}

void MonitoringNodeManager::ReleaseInstance()
{
	SAFEDELETE(sg_Instance);
}

void MonitoringNodeManager::InitMemoryPool()
{
	int iMaxServerNode = 0;
	ioINILoader kLoader( "ls_config_game.ini" );
	{
		kLoader.SetTitle( "MemoryPool" );
		iMaxServerNode = kLoader.LoadInt( "monitoring_pool", 10 );
	}

	{
		kLoader.SetTitle( "Monitoring Session" );
		int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
		
		m_MemNode.CreatePool( 0, iMaxServerNode, FALSE );
		for(int i = 0;i < iMaxServerNode;i++)
		{
			m_MemNode.Push( new MonitoringNode( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 ) );
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][memorypool]Monitoring session send buffer : [%d]", iSendBufferSize );
	}
}

void MonitoringNodeManager::ReleaseMemoryPool()
{
	vMonitoringNode_iter iter, iEnd;	
	iEnd = m_vNode.end();
	for(iter = m_vNode.begin();iter != iEnd;++iter)
	{
		MonitoringNode *pNode = *iter;
		pNode->OnDestroy();
		m_MemNode.Push( pNode );
	}	
	m_vNode.clear();
	m_MemNode.DestroyPool();
}


MonitoringNode *MonitoringNodeManager::CreateNode(SOCKET s)
{
	MonitoringNode *newNode = (MonitoringNode*)m_MemNode.Pop();
	if( !newNode )
	{
		LOG.PrintTimeAndLog(0,"MonitoringNodeManager::CreateServerNode MemPool Zero!");
		return NULL;
	}

	g_iocp.AddHandleToIOCP((HANDLE)s,(DWORD)newNode);

	newNode->SetSocket(s);
	newNode->OnCreate();

	return newNode;
}

void MonitoringNodeManager::AddNode( MonitoringNode *pNewNode )
{
	m_vNode.push_back( pNewNode );
}

void MonitoringNodeManager::RemoveNode( MonitoringNode *pNode )
{
	for(vMonitoringNode::iterator it = m_vNode.begin() ; it != m_vNode.end() ; ++it)
	{
		MonitoringNode* node  = *it;
		if(node && (node == pNode))
		{
			m_vNode.erase(it);
			m_MemNode.Push( node );
			break;
		}
	}
}

void MonitoringNodeManager::ProcessFlush()
{
	if( m_vNode.empty() == false )
	{
		vector< MonitoringNode* >::iterator	iter	= m_vNode.begin();
		vector< MonitoringNode* >::iterator	iterEnd	= m_vNode.end();

		for( iter ; iter != iterEnd ; ++iter )
		{
			MonitoringNode* pMonitoringNode = (*iter);

			if( ! pMonitoringNode->IsActive() )
				continue;
			if( pMonitoringNode->GetSocket() == INVALID_SOCKET )
				continue;

			pMonitoringNode->FlushSendBuffer();
		}
	}
}

void MonitoringNodeManager::SendMessageAllNode( SP2Packet &rkPacket )
{
	vMonitoringNode_iter iter = m_vNode.begin();
	while( iter != m_vNode.end() )
	{
		MonitoringNode *pNode = *iter++;
		pNode->SendMessage( rkPacket );
	}
}