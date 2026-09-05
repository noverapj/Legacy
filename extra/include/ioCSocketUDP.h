#pragma once

#include "ioCSocket.h"

class ioCSocketUDP : public ioCSocket
{
public:
	ioCSocketUDP();
	virtual ~ioCSocketUDP();

public:
	void Init();
	void Destroy();

public:
	BOOL Create();
	BOOL Bind();

	BOOL Connect(const char *IP, int port);
	void Disconnect();

	BOOL Send(const char *buffer, int length);
	BOOL Receive(char *buffer, int& length);

	void SetBindPort(int port) { m_port = port; }
	int GetBindPort() { return m_port; }

public:
	BOOL CreateThread();

protected:
	DWORD m_peerIP;
	DWORD m_peerPort;

	int m_port;
};

