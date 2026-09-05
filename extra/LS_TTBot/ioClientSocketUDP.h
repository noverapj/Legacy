#pragma once

#include "ioCSocketUDP.h"
#include "network/NetworkSecurity.h"

class ioClientSocketUDP : public ioCSocketUDP
{
public:
	ioClientSocketUDP(void);
	~ioClientSocketUDP(void);

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
};
