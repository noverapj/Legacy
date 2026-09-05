#include "../iocpSocketDLL.h"
#include "MPSCRecvQueue.h"


MPSCRecvQueue::MPSCRecvQueue()
{
}

MPSCRecvQueue::~MPSCRecvQueue()
{
	Destroy();
}

void MPSCRecvQueue::Init()
{
	MPSCQueue::Init();
	
}

void MPSCRecvQueue::Destroy()
{
}

void MPSCRecvQueue::SetMemoryPool( DWORD seedCount )
{
	PacketPool::Create( seedCount );
}

int MPSCRecvQueue::PacketParsing()
{
	DWORD count = 0;
	while(TRUE)
	{
		PacketQueue *pPacket = reinterpret_cast<PacketQueue*>(MPSCQueue::Dequeue());
		if(NULL == pPacket) break;
		switch( pPacket->GetType() )
		{
		case PK_QUEUE_SESSION:
			ParseSession( pPacket );
			break;
		case PK_QUEUE_QUERY:
			ParseQuery( pPacket );
			break;
		case PK_QUEUE_INTERNAL:
			ParseInternal( pPacket );
			break;
		case PK_QUEUE_ACCEPT:
			ParseAccept( pPacket );
			break;
		case PK_QUEUE_UDP:
			ParseUDP(pPacket);
			break;
		default:
			PrintTimeAndLog( 0, "MPSCRecvQueue::PacketParsing - Unknown queue type %d",
				pPacket->GetType() );
			break;
		}
		AfterParsing( pPacket );
		++count;
		if(count > RECVCOUNT_MAX)
			break;
	}
	return count;
}

void MPSCRecvQueue::AfterParsing( PacketQueue *queueElem )
{ 
	queueElem->Clear();
	PacketPool::Push(queueElem);
}

bool MPSCRecvQueue::InsertQueue( DWORD node, CPacket &packet, PacketQueueTypes type )
{
	PacketQueue* queueElem = PacketPool::Pop(packet.GetBufferSize());
	if( NULL == queueElem )
	{
		PrintTimeAndLog(0,"RecvQueue::InsertQueue LowMemPool Zero!" );
		return false;
	}
	queueElem->Set( node, packet, type );
	MPSCQueue::Enqueue(reinterpret_cast<NodeData*>(&queueElem->m_nodeEntry));	
	return true;
}

bool MPSCRecvQueue::InsertQueue(DWORD node, CPacket &packet, SOCKET socket)
{
	PacketQueueTypeAccept* queueElem = reinterpret_cast<PacketQueueTypeAccept*>(PacketPool::Pop(packet.GetBufferSize(), TRUE));
	if(NULL == queueElem) return false;
	queueElem->Set( node, packet, PK_QUEUE_ACCEPT );
	queueElem->SetSockHandle( socket );
	MPSCQueue::Enqueue(reinterpret_cast<NodeData*>(&queueElem->m_nodeEntry));	
	return true;
}
