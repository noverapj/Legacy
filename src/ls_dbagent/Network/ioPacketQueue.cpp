#include "../stdafx.h"
#include "ioPacketQueue.h"

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
	SetMemoryPool( 1000 );
}

void ioPacketQueue::ParseSession( PacketQueue *pq )
{
	__try
	{
		CConnectNode *pSessionNode = (CConnectNode*)pq->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_packet.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			pSessionNode->PacketParsing( m_packet );
		}
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParseSession Crash!!" );
	}
}

void ioPacketQueue::ParseQuery( PacketQueue *pq )
{
}

void ioPacketQueue::ParseInternal( PacketQueue *pq )
{
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

			m_packet.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			dwPacketID = m_packet.GetPacketID();

			pCommandNode->PacketParsing( m_packet, acceptor->GetSocketHandle() );
		}
		dwPacketID = 0;
	}
	__except (UnHandledExceptionFilter(GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParseAccept Crash!! - Packet : 0x%x", dwPacketID );
	}
}