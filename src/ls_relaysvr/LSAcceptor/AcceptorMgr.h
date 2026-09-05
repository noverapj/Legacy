#pragma once


#include "../iocpsocketdll\socketmodules\acceptornode.h"

class AcceptorMgr :
	public AcceptorNode
{
public:
	AcceptorMgr(void);
	virtual ~AcceptorMgr(void);

protected:
	void Init();
	void Destory();
	virtual void ReceivePacket( CPacket &packet, SOCKET socket );
	virtual void PacketParsing( CPacket &packet, SOCKET socket );

protected:
	virtual void OnAccept( SP2Packet &packet, SOCKET socket  );
};

