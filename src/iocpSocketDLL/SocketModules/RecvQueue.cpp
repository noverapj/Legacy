#include "../iocpSocketDLL.h"
#include "RecvQueue.h"


RecvQueue::RecvQueue()
{
	Init();
}

RecvQueue::~RecvQueue()
{
	Destroy();
}

void RecvQueue::Init()
{
	cIocpQueue::Startup(1);
}

void RecvQueue::Destroy()
{
}

void RecvQueue::SetMemoryPool( DWORD seedCount )
{
	PacketPool::Create( seedCount );
}

int RecvQueue::PacketParsing()
{
	DWORD bytes = 0, count = 0;
	while(TRUE)
	{
		PacketQueue *pPacket = reinterpret_cast<PacketQueue*>(cIocpQueue::Dequeue(bytes));
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
			PrintTimeAndLog( 0, "RecvQueue::PacketParsing - Unknown queue type %d",
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

void RecvQueue::AfterParsing( PacketQueue *queueElem )
{
	int packetType = queueElem->GetType();

	queueElem->Clear();
	PacketPool::Push(queueElem);
}

bool RecvQueue::InsertQueue( DWORD node, CPacket &packet, PacketQueueTypes type )
{
	PacketQueue* queueElem = PacketPool::Pop(packet.GetBufferSize());
	if( NULL == queueElem )
	{
		PrintTimeAndLog(0,"RecvQueue::InsertQueue LowMemPool Zero!" );
		return false;
	}

	queueElem->Set( node, packet, type );
	
	cIocpQueue::Enqueue( reinterpret_cast<DWORD>(queueElem), sizeof(PacketQueue) );	
	return true;
}

bool RecvQueue::InsertQueue(DWORD node, CPacket &packet, SOCKET socket)
{
	PacketQueueTypeAccept* queueElem = reinterpret_cast<PacketQueueTypeAccept*>(PacketPool::Pop(packet.GetBufferSize(), TRUE));
	if(NULL == queueElem) return false;

	queueElem->Set( node, packet, PK_QUEUE_ACCEPT );
	queueElem->SetSockHandle( socket );
	cIocpQueue::Enqueue( reinterpret_cast<DWORD>(queueElem), sizeof(PacketQueue) );	
	return true;
}
