#include "../stdafx.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "ioPacketQueue.h"
#include "../Operation/LSLogicHandler.h"
#include "../Operation/LSLogicOperations.h"

extern CLog LOG;

ioPacketQueue *ioPacketQueue::sg_Instance = NULL;

ioPacketQueue::ioPacketQueue()	{}
ioPacketQueue::~ioPacketQueue()	{}

ioPacketQueue &ioPacketQueue::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioPacketQueue;
	return *sg_Instance;
}

void ioPacketQueue::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioPacketQueue::Initialize()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "RecvQueue" );
	DWORD dwFirstRecvQueue = kLoader.LoadInt( "FirstQueue", 100 );
	SetMemoryPool( dwFirstRecvQueue );

	m_handler = new LSLogicHandler(this);
}

void ioPacketQueue::ParseSession( PacketQueue *pq )
{
	int connectType = 0;
	__try
	{
		//LOG.PrintTimeAndLog(0,"[TID]I'm ParseSession:%d\n",GetCurrentThreadId());
		//서버 패킷 처리
		CConnectNode *pSessionNode = reinterpret_cast<CConnectNode*>(pq->GetNode());
		connectType = pSessionNode->GetConnectType();
		//LOG.PrintTimeAndLog( 0, "CConnectNode *pSessionNode  Crash(%d)!!",pq->GetNode());
		//LOG.PrintTimeAndLog(0,"asdasd(%s:%d) %d" , (int)pSessionNode->IsActive(),pSessionNode->GetPublicIP(),pSessionNode->GetUDP_port());
		if( pSessionNode && pSessionNode->IsActive() )
		{
			//LOG.PrintTimeAndLog( 0, "pSessionNode && pSessionNode->IsActive() (%d)!!",pq->GetNode());
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			pSessionNode->PacketParsing( m_SessionPacket );
				//LOG.PrintTimeAndLog( 0, "acketParsing( m_SessionPacket ); Crash!!" );
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "[error][queue]ParseSession Crash!! connectType=%d PacketID%x",connectType,m_SessionPacket.GetPacketID() );
	}
}

void ioPacketQueue::ParseQuery( PacketQueue *pq )
{
}

void ioPacketQueue::ParseInternal( PacketQueue *packetQueue )
{
	__try
	{

		if(packetQueue->GetNode() == NULL)
		{
			m_SessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );

			stOp_* stDistinc = (stOp_*)m_SessionPacket.GetBuffer()+sizeof(int);  
			Operation* op = m_handler->FindOperation(stDistinc->opid);
			if(op)
				op->Run((void*)stDistinc);
		}
		else
		{
			DWORD dwPacketID = 0;
			CCommandNode *pCommandNode = (CCommandNode*)packetQueue->GetNode();
			if( pCommandNode )
			{
				m_SessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );
				dwPacketID = m_SessionPacket.GetPacketID();
		 
				pCommandNode->PacketParsing( m_SessionPacket );
			}
			dwPacketID = 0;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParsInter Crash!!" );
	}
}

void ioPacketQueue::ParseAccept( PacketQueue *pq )
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)pq->GetNode();
		if( pCommandNode )
		{
			PacketQueueTypeAccept* acceptor = (PacketQueueTypeAccept*)pq;

			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			dwPacketID = m_SessionPacket.GetPacketID();

			pCommandNode->PacketParsing( m_SessionPacket, acceptor->GetSocketHandle() );
		}
		dwPacketID = 0;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParseAccept Crash!! - Packet : 0x%x", dwPacketID );
	}
}

bool ioPacketQueue::InsertQueue( DWORD node, CPacket &packet, PacketQueueTypes type )
{
	if(!MPSCRecvQueue::InsertQueue(node, packet, type))
	{
		LOG.PrintTimeAndLog( 0, "%s LowMemPool Error Exit", __FUNCTION__ );
	//	g_App.Shutdown(SHUTDOWN_EMPTYPOOL);
		return false;
	}
	return true;
}

bool ioPacketQueue::InsertQueue( DWORD node, CPacket &packet, SOCKET socket )
{
	if(!MPSCRecvQueue::InsertQueue(node, packet, socket))
	{
		return false;
	}
	return true;
}
