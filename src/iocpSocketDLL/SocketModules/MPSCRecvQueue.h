#pragma once

#include "../../include/MemPooler.h"
#include "PacketPool.h"
#include "MPSCQueue.h"
#include "PacketQueue.h"

class IOCP_SOCKET_API MPSCRecvQueue : public MPSCQueue, 
									  public PacketPool									 
{
public:
	void Init();
	void SetMemoryPool( DWORD seedCount );
	virtual bool InsertQueue(DWORD node, CPacket &packet, PacketQueueTypes type);
	virtual bool InsertQueue(DWORD node, CPacket &packet, SOCKET socket);

public:
	int PacketParsing();
	void AfterParsing( PacketQueue *queueElem );

	virtual void ParseSession( PacketQueue *pq ) = 0;
	virtual void ParseQuery( PacketQueue *pq ) = 0;
	virtual void ParseInternal( PacketQueue *pq ) = 0;
	virtual void ParseAccept( PacketQueue *pq ) = 0;
	virtual void ParseUDP(PacketQueue* pq) {};

protected:
	void Destroy();

	MPSCRecvQueue();
	virtual ~MPSCRecvQueue();
 
};

