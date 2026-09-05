#include "../stdafx.h"
#include "ConnectAssist.h"
#include "GameServer.h"
#include "../WemadeBillingServer/WemadeBuyServer.h"
#include "../MgameBillingServer/MgameBillingServer.h"
#include "../ToonilandBillingServer/ToonilandBillingServer.h"
#include "../NexonNISMSServer/NexonNISMSServer.h"
#include "../NexonEUSessionServer/NexonEUSessionServer.h"
#include "../Network/ioPacketQueue.h"

#define ELAPS_TIME 2000

extern CLog LOG;


ConnectAssist::ConnectAssist(void)
{
	Init();
}

ConnectAssist::~ConnectAssist(void)
{
}

void ConnectAssist::Init()
{
	cIocpQueue::Startup();

	SetRunState(true);
}

void ConnectAssist::Destroy()
{
	SetRunState(false);
}

void ConnectAssist::Run()
{

	DWORD size;
	int startTime = 0,endTime = 0,elapsedTime = 0;

	while(GetRunState())
	{
		CConnectNode* connectNode = (CConnectNode*) cIocpQueue::Dequeue(size);

		if(connectNode) //코드 정리 필요 보기 별로임 
		{
			startTime = GetCurrentTime();

			if(Connect(connectNode))
					PushLogicThread(connectNode);
			else
			{
			//	LOG.PrintTimeAndLog( 0,"type[%d] Connect Error",connectNode->GetConnectType() );
				PushNode(connectNode);
			}
			endTime = GetCurrentTime();

			elapsedTime = endTime - startTime;

			if(elapsedTime <= ELAPS_TIME)//1초
				Sleep(ELAPS_TIME-elapsedTime);

		}

	}
}

bool ConnectAssist::Connect( CConnectNode* connectNode )
{
	bool connectState = false;
	switch(connectNode->GetConnectType())//kyg int2type ?
	{
	case CONNECT_TYPE_MGAME_BILLING_SERVER:
		 connectState = ConnectTo<MgameBillingServer>(connectNode);
		break;
	case CONNECT_TYPE_TOONILAND_BILLING_SERVER:
		connectState = ConnectTo<ToonilandBillingServer>(connectNode);
		break;
	case CONNECT_TYPE_WEMADE_BUY_SERVER:
		connectState = ConnectTo<WemadeBuyServer>(connectNode);
		break;
	case CONNECT_TYPE_NEXON_SESSION_SERVER:
		connectState = ConnectTo<NexonSessionServer>(connectNode);
		break;
	case CONNECT_TYPE_NEXON_BUY_SERVER:
		//ConnectTo<MgameBillingServer>(connectNode);
		break;
	case CONNECT_TYPE_NEXON_EU_SERVER:
		connectState = ConnectTo<NexonNISMSServer>(connectNode);
		break;

	case CONNECT_TYPE_NEXON_EU_SESSION_SERVER:
		connectState = ConnectTo<NexonEUSessionServer>(connectNode);
		break;
	}

	return connectState;
}

void ConnectAssist::PushNode( CConnectNode* connectNode )
{
	DWORD key = (DWORD)connectNode;
	cIocpQueue::Enqueue(key,sizeof(connectNode));
}

void ConnectAssist::PushLogicThread( CConnectNode* connectNode )
{
	OnConnect_ onConnectOperation;

	onConnectOperation.node = connectNode;

	OPERATIONRUN(onConnectOperation);
	 
}

