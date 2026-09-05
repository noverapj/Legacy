#pragma once

class AcceptorNode;

class AcceptorUserNode : public AcceptorNode
{
public:
	AcceptorUserNode(void);
	~AcceptorUserNode(void);

	void Init();
	void Destroy();

protected:
	virtual void ReceivePacket( CPacket &packet, SOCKET socket );
	virtual void PacketParsing( CPacket &packet, SOCKET socket );

protected:
	virtual void OnAccept( SP2Packet &packet, SOCKET socket  );
};
