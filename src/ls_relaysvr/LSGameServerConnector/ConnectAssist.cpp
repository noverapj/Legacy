#include "StdAfx.h"
#include "ConnectAssist.h"


ConnectAssist::ConnectAssist(void)
{
}


ConnectAssist::~ConnectAssist(void)
{
}

void ConnectAssist::Init()
{
	int sendMaxSize = g_Config()->GetConnectMgrSendSize();
	int readMaxSize = g_Config()->GetConnectMgrRecvSize();
	
	
}

void ConnectAssist::Destroy()
{

}

void ConnectAssist::InitMemoryPool()
{
	int maxSize  = g_Config()->GetGameServerMaxSize();
	for(int i=0; i< maxSize*2; ++i)
	{
		GameServerNode* node = new GameServerNode;
		if(node)
			m_memPool.Push(node);
		else
		{
			LOG.PrintTimeAndLog(0,"Error! ConnectAssist");
		}
	}
	
}

Connect_* ConnectAssist::GetQueue()
{
	return m_queue.pop(true);
}

void ConnectAssist::PutQueue(Connect_* data)
{
	m_queue.push(data);
}

void ConnectAssist::PutConnectData( int opid, SVRCONNECTINFO_& addr, GameServerNode* node /*= NULL*/ )
{
	Connect_* data = (Connect_*)g_OPMemPool()->Pop(sizeof(Connect_));

	strcpy_s(data->ipAddr,addr.ipAddr);
	data->port = addr.port;
	data->opid = opid;
	data->node = node;
	data->serverIndex = addr.serverIndex;

	PutQueue(data);
}

void ConnectAssist::ConnectClient(SVRCONNECTINFO_& addr)
{
	GameServerNode* node = m_memPool.Pop();	 

	if(node == NULL)
	{
		LOG.PrintTimeAndLog(0,"ConnectClient Error Clinet MemoryPool Empty");
		return;
	}

	if(!node->ConnectTo(addr.ipAddr, addr.port)) // 이부분을 putq 해줌 
	{		 
		LOG.PrintTimeAndLog(0,"ServerConnectorMgr Connect Error %s:%d",addr.ipAddr,addr.port);
		Debug(_T("ServerConnectorMgr Connect Error %s:%d\n"),addr.ipAddr,addr.port);
		
		node->SetSendServerId(addr.serverIndex);
		m_memPool.Push(node);
	
		node = NULL;

		CreateTimer(addr);
		 
	}
	else
	{
		Debug(_T("ConnectAssist : (%s, %d)connected\r\n"), addr.ipAddr, addr.port);
		node->SetSendServerId(addr.serverIndex);

		PushClient(node); 
	}
}

void ConnectAssist::PushClient(GameServerNode* node)
{
	OnAccept_ stAccept;

	stAccept.opid = OperationIndex::ONACCEPT;
	stAccept.node = node;

	PUTQFUNNC(stAccept,(g_Queue()));
}

void ConnectAssist::CreateTimer(SVRCONNECTINFO_& addr)
{
	ReConnect_* stTmp = (ReConnect_*)(g_OPMemPool()->Pop(sizeof(ReConnect_)));

	stTmp->opid = OperationIndex::RECONNECT;
	stTmp->serverPort = addr.port;
	stTmp->serverIndex = addr.serverIndex;
	strcpy_s(stTmp->ipAddr,addr.ipAddr);

	g_TimerMgr()->AddTimer(false,g_Config()->GetConnectTime(),stTmp);	
}

void ConnectAssist::Run()
{
	while(1)
	{
		Connect_* data = GetQueue();
	 
		if(data)
		{
			SVRCONNECTINFO_ addr;

			strcpy_s(addr.ipAddr,data->ipAddr);
			addr.port			= data->port;
			addr.serverIndex	= data->serverIndex;
	
			switch(data->opid)
			{
			case ConnectAssistTypes::CONNECT:
				{
					ConnectClient(addr);
				}
				break;

			case ConnectAssistTypes::CLOSE:
				{
					if(data->node!= NULL)
					{
						m_memPool.Push(data->node);
						data->node = NULL;
					}
					g_ConnectAssist()->PutConnectData(ConnectAssistTypes::CONNECT,addr);
				}
				break;

			default:
				LOG.PrintTimeAndLog(0,"Unknown ConnectAssist Operation :%d",data->opid);
				break;
			}
		}
		g_OPMemPool()->Push(data);
	}
}