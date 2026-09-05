#pragma once

class MPSCRecvQueue;

class ioPacketQueue : public MPSCRecvQueue
{
	static ioPacketQueue *sg_Instance;

public:
	static ioPacketQueue &GetInstance();
	static void ReleaseInstance();

protected:
	CPacket m_SessionPacket;
	CPacket m_QueryPacket;
	int     m_iAddQueueCount;
	int     m_iAddLowQueueCount;
	int     m_iAddHighQueueCount;

public:
	virtual void ParseInternal( PacketQueue *pq );
	virtual void ParseSession( PacketQueue *pq );
	virtual void ParseQuery( PacketQueue *pq );
	virtual void ParseAccept( PacketQueue *pq );
	virtual void ParseUDP(PacketQueue* pq); //UDPNode에서 들어오는 패킷
	virtual bool InsertQueue( DWORD node, CPacket &packet, PacketQueueTypes type );
	virtual bool InsertQueue( DWORD node, CPacket &packet, SOCKET socket );

public:
	void Initialize();
	void DeleteUserQueue();
	int  GetAddQueueCount(){ return m_iAddQueueCount; }

private: /* Singleton */
	ioPacketQueue();
	virtual ~ioPacketQueue();
};

#define g_RecvQueue  ioPacketQueue::GetInstance()