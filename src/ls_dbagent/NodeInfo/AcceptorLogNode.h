#pragma once


class AcceptorNode;
class SP2Packet;

class AcceptorLogNode : AcceptorNode
{
public:
	AcceptorLogNode(void);
	~AcceptorLogNode(void);

public:
	void Init();
	void Destroy();

protected:
	virtual void ReceivePacket( CPacket &packet, SOCKET socket );
	virtual void PacketParsing( CPacket &packet, SOCKET socket );

protected:
	virtual void OnAccept( SP2Packet &packet, SOCKET socket  );

protected:
	void SetSocketOpation( SOCKET& socket );

};


