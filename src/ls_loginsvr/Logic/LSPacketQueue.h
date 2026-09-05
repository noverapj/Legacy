#pragma once
class LSLogicHandler;
class LSPacketQueue :
	public MPSCRecvQueue //kyg recvqueue¿¡¼­ ¼öÁ¤µÊ 
{
public:
	LSPacketQueue(void);
	virtual ~LSPacketQueue(void);

	

public:
	void Initz();
	 
	virtual void ParseSession( PacketQueue *queue );
	virtual void ParseQuery( PacketQueue *queue );
	virtual void ParseInternal( PacketQueue *queue );
	virtual void ParseAccept( PacketQueue *queue );
	const LSLogicHandler* GetHandler(){ return m_handler; }

private:
	CPacket m_sessionPacket;
	LSLogicHandler* m_handler;
};

