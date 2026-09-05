#include "StdAfx.h"
#include "LSPacketQueue.h"
#include "LSLogicHandler.h"

LSPacketQueue::LSPacketQueue(void)
{
}


LSPacketQueue::~LSPacketQueue(void)
{
}

void LSPacketQueue::InitPacketQueue()
{
	InitMemoryPool();
	SetLogicHandler();
}

void LSPacketQueue::InitMemoryPool()
{
	SetMemoryPool(g_Config()->GetQueueFirstRecvQueue());
}

void LSPacketQueue::SetLogicHandler()
{
	m_handler = new LSLogicHandler(this);
}

void LSPacketQueue::ParseAccept(PacketQueue *packetQueue)
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)packetQueue->GetNode();

		if( pCommandNode )
		{
			PacketQueueTypeAccept* acceptor = reinterpret_cast<PacketQueueTypeAccept*>(packetQueue);
			m_sessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );
			dwPacketID = m_sessionPacket.GetPacketID();
			pCommandNode->PacketParsing( m_sessionPacket, acceptor->GetSocketHandle() );
		}
		dwPacketID = 0;
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
	}
}
 
void LSPacketQueue::ParseQuery(PacketQueue *packetQueue)
{
	DWORD dwPacketID = 0;
	__try
	{
		 
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}
}

void LSPacketQueue::ParseInternal(PacketQueue *packetQueue)
{ 
  	__try
	{
		 
		if(packetQueue->GetNode() == NULL)
		{
			stOp_* stDistinc = (stOp_*)packetQueue->GetBuffer()+sizeof(int);  
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
				m_sessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );
				dwPacketID = m_sessionPacket.GetPacketID();
				g_PacketChecker()->SessionPacket( m_sessionPacket.GetPacketID() );
				g_PacketChecker()->PacketSizeCheck( m_sessionPacket.GetPacketID(), m_sessionPacket.GetBufferSize() );
				pCommandNode->PacketParsing( m_sessionPacket );
			}
			dwPacketID = 0;

		}
	}
 	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}

}

void LSPacketQueue::ParseSession(PacketQueue *packetQueue)
{	  
 	DWORD dwPacketID = 0;
	__try
	{
		CConnectNode *pSessionNode = (CConnectNode*)packetQueue->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_sessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );
			pSessionNode->PacketParsing( m_sessionPacket );
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
	}
}

void LSPacketQueue::ParseUDP( PacketQueue* packetQueue )
{
	__try
	{
		UDPNode *pSessionNode = (UDPNode*)packetQueue->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_sessionPacket.SetBufferCopy( packetQueue->GetBuffer(), packetQueue->GetSize(), packetQueue->GetPosition() );
			pSessionNode->PacketParsing( m_sessionPacket );
		}
	}
	__except (UnHandledExceptionFilter (GetExceptionInformation()))
	{
	}
}

 