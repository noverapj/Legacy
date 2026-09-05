#include "StdAfx.h"
#include "LSPacketQueue.h"
#include "LSLogicHandler.h"

LSPacketQueue::LSPacketQueue(void)
{
}


LSPacketQueue::~LSPacketQueue(void)
{
}

void LSPacketQueue::ParseAccept(PacketQueue *queue)
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)queue->GetNode();
		if( pCommandNode )
		{
			PacketQueueTypeAccept* acceptor = (PacketQueueTypeAccept*)queue;
			m_sessionPacket.SetBufferCopy( queue->GetBuffer(), queue->GetSize(), queue->GetPosition() );
			dwPacketID = m_sessionPacket.GetPacketID();
			pCommandNode->PacketParsing( m_sessionPacket, acceptor->GetSocketHandle() );
		}
		dwPacketID = 0;
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
	}
}
void LSPacketQueue::Initz()
{
	SetMemoryPool(g_Config()->NQueueFirstRecvQueue());
	m_handler = new LSLogicHandler(this);
}
 
void LSPacketQueue::ParseQuery(PacketQueue *queue)
{
	DWORD dwPacketID = 0;
	__try
	{
		 
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}
}
void LSPacketQueue::ParseInternal(PacketQueue *queue)
{ 
  	__try
	{
		if(queue->GetNode() == NULL)
		{
			stOp_* stDistinc = (stOp_*)queue->GetBuffer()+sizeof(int);  
			Operation* op = m_handler->FindOperation(stDistinc->opid);
			if(op)
				op->Run((void*)stDistinc);
		}
		else
		{
			DWORD dwPacketID = 0;
			CCommandNode *pCommandNode = (CCommandNode*)queue->GetNode();
			if( pCommandNode )
			{
				m_sessionPacket.SetBufferCopy( queue->GetBuffer(), queue->GetSize(), queue->GetPosition() );
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
void LSPacketQueue::ParseSession(PacketQueue *queue)
{	  
 	DWORD dwPacketID = 0;
	__try
	{
		CConnectNode *pSessionNode = (CConnectNode*)queue->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_sessionPacket.SetBufferCopy( queue->GetBuffer(), queue->GetSize(), queue->GetPosition() );
			pSessionNode->PacketParsing( m_sessionPacket );
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
	}
}

 