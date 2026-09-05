#pragma once

class AcceptorNode;

class AcceptorMonitorNode : public AcceptorNode
{
public:
	AcceptorMonitorNode(void);
	~AcceptorMonitorNode(void);

	void Init();
	void Destroy();

protected:
	virtual void ReceivePacket( CPacket &packet, SOCKET socket );
	virtual void PacketParsing( CPacket &packet, SOCKET socket );

protected:
	void OnAccept( SP2Packet &packet, SOCKET socket );
};
