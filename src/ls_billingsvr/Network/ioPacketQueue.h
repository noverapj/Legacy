#pragma once

class MPSCRecvQueue;
class LSLogicHandler;

class ioPacketQueue : public MPSCRecvQueue
{
	static ioPacketQueue *sg_Instance;

protected:
	CPacket m_SessionPacket;
	CPacket m_QueryPacket;
	LSLogicHandler* m_handler;
public:
	static ioPacketQueue &GetInstance();
	static void ReleaseInstance();

public:
	virtual void ParseInternal( PacketQueue *pq );
	virtual void ParseSession( PacketQueue *pq );
	virtual void ParseQuery( PacketQueue *pq );
	virtual void ParseAccept( PacketQueue *pq );
	virtual bool InsertQueue( DWORD node, CPacket &packet, PacketQueueTypes type );
	virtual bool InsertQueue( DWORD node, CPacket &packet, SOCKET socket );

public:
	void Initialize();

private: /* Singleton */
	ioPacketQueue();
	virtual ~ioPacketQueue();
};

#define g_RecvQueue  ioPacketQueue::GetInstance()