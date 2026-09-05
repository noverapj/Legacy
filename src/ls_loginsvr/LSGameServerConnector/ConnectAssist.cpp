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
	int send = g_Config()->NCMgrSendSize();
	int read = g_Config()->NCMgrRecvSize();
	int max  = g_Config()->NGameServerMax();
	m_memPool.CreatePool(max,max+1);
	//m_memPool.CreatePool(0,max);
#if 0
	for(int i=0; i< max; ++i)
	{
		m_memPool.CreatePush(new LSConnector(INVALID_SOCKET,send,read));//kyg 변환 
	}
#endif
}
void ConnectAssist::Finit()
{

}
Connect_* ConnectAssist::GetQueue()
{
	return m_queue.pop(true);
}

void ConnectAssist::PutQueue(Connect_* data)
{
	m_queue.push(data);
}

void ConnectAssist::PutConnectData( int opid, SVRCONNECTINFO_& addr, LSConnector* node /*= NULL*/ )
{
	Connect_* data		= (Connect_*)g_OPMemPool()->Pop(sizeof(Connect_));
	data->opid			= opid;
	data->node			= node;
	data->port			= addr.port;
	data->serverIndex	= addr.serverIndex;
	strcpy_s(data->ipAddr, addr.ipAddr);

	PutQueue(data);
}

void ConnectAssist::ConnectClient(SVRCONNECTINFO_& addr)
{
	LSConnector* node = m_memPool.Pop();	 
	if(node == NULL)
	{
		LOG.PrintTimeAndLog(0,"ConnectClient Error Clinet MemoryPool Empty");
		return;
	}

	if(!node->ConnectTo(addr.ipAddr, addr.port)) // 이부분을 putq 해줌 
	{		 
		LOG.PrintTimeAndLog(0,"ServerConnectorMgr Connect Error %s:%d",addr.ipAddr,addr.port);
		Debug(_T("ServerConnectorMgr Connect Error %s:%d\n"),addr.ipAddr,addr.port);

		node->m_sendServerId = addr.serverIndex;
		m_memPool.Push(node);
		CreateTimer(addr);
	}
	else
	{
 		Debug(_T("ConnectAssist : (%s, %d)connected\r\n"), addr.ipAddr, addr.port);
		node->m_sendServerId = addr.serverIndex;
		PushClient(node); 
	}
}

void ConnectAssist::PushClient(LSConnector* node)
{
	OnAccept_ stAccept;
	stAccept.opid = EOPID::ONACCEPT;
	stAccept.node = node;
	PUTQFUNNC(stAccept,g_Queue());

}

void ConnectAssist::CreateTimer(SVRCONNECTINFO_& addr)
{
	ReConnect_* stTmp = (ReConnect_*)(g_OPMemPool()->Pop(sizeof(ReConnect_)));
	stTmp->opid = EOPID::RECONNECT;
	stTmp->serverPort = addr.port;
	stTmp->serverIndex = addr.serverIndex;
	strcpy_s(stTmp->ipAddr,addr.ipAddr);
	g_TimerMgr()->AddTimer(false,g_Config()->ConnectTime(),stTmp);	
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
			case ECONNECTASSIST::CONNECT:
				{
					ConnectClient(addr);
				}
				break;
			case ECONNECTASSIST::CLOSE:
				{
					if(data->node!= NULL)
					{
						m_memPool.Push(data->node);
						data->node = NULL;
					}
					g_ConnectAssist()->PutConnectData(ECONNECTASSIST::CONNECT,addr);
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