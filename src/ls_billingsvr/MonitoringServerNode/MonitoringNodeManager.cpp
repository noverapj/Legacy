#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/iocpHandler.h"
#include "./monitoringnodemanager.h"


extern CLog LOG;

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
	
	ioINILoader kLoader( "ls_config_billingsvr.ini" );

	kLoader.SetTitle( "MemoryPool" );
	iMaxServerNode = kLoader.LoadInt( "monitoring_pool", 10 );
	

	kLoader.SetTitle( "Monitoring Session Buffer" );
	int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	


	for(int i = 0; i < iMaxServerNode * 2; i++) //kyg ??? ?? 
	{
		m_MemNode.Push( new MonitoringNode( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 ) );
	}
	LOG.PrintTimeAndLog( 0, "Monitoring Session SendBuffer : %d", iSendBufferSize );
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
		{
			ioINILoader kLoader( "ServerMemoryInfo.ini" );
			kLoader.SetTitle( "Monitoring Session" );
			int iSendBufferSize = kLoader.LoadInt( "SendBufferSize", MAX_BUFFER );	
			m_MemNode.Push( new MonitoringNode( INVALID_SOCKET, iSendBufferSize, MAX_BUFFER * 2 ) );
			LOG.PrintTimeAndLog( 0, "[error][monitoring]MonitoringNodeManager::CreateNode MemPool Zero by Add Pool" );
		}

		newNode = (MonitoringNode*)m_MemNode.Pop();
		if( !newNode )
		{
			LOG.PrintTimeAndLog(0,"[error][monitoring]MonitoringNodeManager::CreateServerNode MemPool Zero!");
			return NULL;
		}
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

void MonitoringNodeManager::Node_Destroy()
{
	//FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	//if( m_vNode.empty() ) return;

	//vMonitoringNode_iter iter = m_vNode.begin();
	//while( iter != m_vNode.end() )
	//{
	//	MonitoringNode *pNode = *iter;
	//	if( pNode->IsDisconnectState() )
	//	{
	//		pNode->OnSessionDestroy();
	//		iter = m_vNode.erase( iter );
	//		m_MemNode.Push( pNode );			
	//	}
	//	else
	//		iter++;
	//}
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