#pragma once

class MPSCRecvQueue;

class ioPacketQueue : public MPSCRecvQueue
{
	static ioPacketQueue *sg_Instance;

protected:
	CPacket m_SessionPacket;
	CPacket m_QueryPacket;

public:
	static ioPacketQueue &GetInstance();
	static void ReleaseInstance();

public:
	virtual void ParseInternal( PacketQueue *pq );
	virtual void ParseSession( PacketQueue *pq );
	virtual void ParseQuery( PacketQueue *pq );
	virtual void ParseAccept( PacketQueue *pq );

public:
	void Initialize();

private: /* Singleton */
	ioPacketQueue();
	virtual ~ioPacketQueue();
};

#define g_RecvQueue  ioPacketQueue::GetInstance()