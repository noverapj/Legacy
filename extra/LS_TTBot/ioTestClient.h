#pragma once

#include "ioClientSocketTCP.h"
#include "ioClientSocketUDP.h"
#include "network/SP2Packet.h"

class ioTestClient
{
public:
	ioTestClient(void);
	~ioTestClient(void);

	void Init();
	void Destroy();

public:
	void Startup(const char* IP, const uint32 port);

	BOOL DoConnect(const BOOL thread = TRUE);
	BOOL DoDisconnect();

public:
	void SendTCP(SP2Packet& packet);
	void SendUDP(SP2Packet& packet);

protected:
	ioClientSocketTCP m_socketTCP;
	ioClientSocketUDP m_socketUDP;

	uint32 m_port;
	CString m_IP;
};
