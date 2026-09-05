#pragma once


class LSLogicHandler;

class LSPacketQueue :
	public MPSCRecvQueue //kyg recvqueue에서 수정됨 
{
public:
	LSPacketQueue(void);
	virtual ~LSPacketQueue(void);

public:
	void InitPacketQueue();
public:
	void InitMemoryPool();
	void SetLogicHandler();

public:
	virtual void ParseSession( PacketQueue *packetQueue );
	virtual void ParseQuery( PacketQueue *packetQueue );
	virtual void ParseInternal( PacketQueue *packetQueue );
	virtual void ParseAccept( PacketQueue *packetQueue );
	virtual void ParseUDP(PacketQueue* packetQueue); //UDPNode에서 들어오는 패킷

public:
	const LSLogicHandler* GetHandler(){ return m_handler; }

private:
	CPacket m_sessionPacket;
	LSLogicHandler* m_handler;
};

