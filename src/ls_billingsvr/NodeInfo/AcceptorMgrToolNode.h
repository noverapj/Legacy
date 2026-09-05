#pragma once

class AcceptorNode;
class SP2Packet;

class AcceptorMgrToolNode : public AcceptorNode
{
public:
	AcceptorMgrToolNode(void);
	~AcceptorMgrToolNode(void);

	void Init();
	void Destroy();

protected:
	virtual void ReceivePacket( CPacket &packet, SOCKET socket );
	virtual void PacketParsing( CPacket &packet, SOCKET socket );

protected:
	virtual void OnAccept( SP2Packet &packet, SOCKET socket  );
};
