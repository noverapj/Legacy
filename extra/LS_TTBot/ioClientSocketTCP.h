#pragma once

#include "ioCSocketTCP.h"
#include "cBuffer.h"
#include "network/SP2Packet.h"
#include "network/NetworkSecurity.h"

class ioClientSocketTCP : public ioCSocketTCP
{
public:
	ioClientSocketTCP(void);
	~ioClientSocketTCP(void);

	void Init();
	void Destroy();

public:
	void OnConnect();
	void OnDisconnect();
	void OnSend();
	void OnReceive(char* buffer, int& length);

	void BeforeSend(CPacket &rkPacket);
	void AfterSend(CPacket &rkPacket);
	BOOL SendPacket(CPacket &rkPacket);

	BOOL CheckNS( CPacket &rkPacket );
	void SetNS(NetworkSecurity* ns)	{ m_NS = ns; }

protected:
	NetworkSecurity *m_NS;
	cBuffer *m_storage;
};
