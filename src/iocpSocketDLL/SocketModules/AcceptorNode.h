#pragma once

#include "ConnectNode.h"

class IOCP_SOCKET_API AcceptorNode : public CCommandNode
{
public:
	AcceptorNode(void);
	~AcceptorNode(void);

	void Init();
	void Destroy();

public:
	virtual void ReceivePacket( CPacket &packet, SOCKET socket ) = 0;
};
