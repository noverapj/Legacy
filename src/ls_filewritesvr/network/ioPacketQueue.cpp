#include "stdafx.h"

#include "ioPacketQueue.h"
#include "../ioMainProcess.h"
#include "../Shutdown.h"
#include "../../iocpSocketDLL/SocketModules/UDPNode.h"

ioPacketQueue *ioPacketQueue::sg_Instance = NULL;
ioPacketQueue::ioPacketQueue() : m_iAddQueueCount( 0 ), m_iAddLowQueueCount( 0 ), m_iAddHighQueueCount( 0 )
{
}

ioPacketQueue::~ioPacketQueue()
{
}

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
	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "Packet Recv Queue" );
	DWORD dwFirstMPSCRecvQueue = kLoader.LoadInt( "FirstQueue", 1000 );
	DWORD dwExtendMPSCRecvQueue   = kLoader.LoadInt( "ExtendQueue", 100 );
	SetMemoryPool( dwFirstMPSCRecvQueue );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Packet Recv Queue : First[%d] - Extend[%d]", dwFirstMPSCRecvQueue, dwExtendMPSCRecvQueue );
 
}

void ioPacketQueue::ParseSession( PacketQueue *pq )
{
	DWORD dwPacketID = 0;
	__try
	{
		CConnectNode *pSessionNode = (CConnectNode*)pq->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition()  );
			dwPacketID = m_SessionPacket.GetPacketID();

			NetLOG.PrintNoEnterLog( 0, "[0x%x]", dwPacketID );

			pSessionNode->PacketParsing( m_SessionPacket );
		}
		dwPacketID = 0;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ParseSession Crash!! - Packet : 0x%x", dwPacketID );

	}
}

void ioPacketQueue::ParseQuery( PacketQueue *pq )
{
	//DWORD dwPacketID = 0;
	//__try
	//{
	//	m_QueryPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition()  );
	//	dwPacketID = m_QueryPacket.GetPacketID();
	//	g_UserNodeManager.GlobalQueryParse((SP2Packet&)m_QueryPacket);
	//	g_PacketChecker.PacketSizeCheck( m_QueryPacket.GetPacketID(), m_QueryPacket.GetBufferSize() );
	//	dwPacketID = 0;
	//}
	//__except (ExceptCallBack (GetExceptionInformation()))
	//{
	//	//
	//	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ParseQuery Crash!! - Packet : 0x%x", dwPacketID );
	//}
}

void ioPacketQueue::ParseInternal( PacketQueue *pq )
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)pq->GetNode();
		if( pCommandNode )
		{
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			dwPacketID = m_SessionPacket.GetPacketID();
			pCommandNode->PacketParsing( m_SessionPacket );
		}
		dwPacketID = 0;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ParseSession Crash!! - Packet : 0x%x", dwPacketID);
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ParseSession Crash!! - Packet : 0x%x - UDP Line : %d", dwPacketID );
	}
}
void ioPacketQueue::DeleteUserQueue()
{
}

bool ioPacketQueue::InsertQueue(DWORD node, CPacket &packet, PacketQueueTypes type)
{
	if(!MPSCRecvQueue::InsertQueue(node, packet, type))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s LowMemPool Error Exit", __FUNCTION__ );
		g_App.Shutdown(SHUTDOWN_EMPTYPOOL);
		return false;
	}
	return true;
}

bool ioPacketQueue::InsertQueue(DWORD node, CPacket &packet, SOCKET socket)
{
	if(!MPSCRecvQueue::InsertQueue(node, packet, socket))
	{
		return false;
	}
	return true;
}

void ioPacketQueue::ParseUDP( PacketQueue* pq )
{
	__try
	{
		UDPNode *pSessionNode = (UDPNode*)pq->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			pSessionNode->PacketParsing( m_SessionPacket );
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
	}

}

