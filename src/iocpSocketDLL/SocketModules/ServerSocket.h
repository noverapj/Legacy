// ServerSocket.h: interface for the ServerSocket class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../ThreadModules/Thread.h"

class CPacket;
class AcceptorNode;

class IOCP_SOCKET_API ServerSocket : public Thread    
{
	
public:
	ServerSocket();
	virtual ~ServerSocket();

	void Init();
	void Destroy();

public:
	void CloseSocket();

	bool Start(const char *iip, const int port, const int backlog = SOMAXCONN);

	void SetAcceptor(AcceptorNode* node, const uint32 packetId);

	SOCKET AcceptConnection(const uint32 time);
	void AcceptSession(SOCKET socket);

protected:
	void Run();

protected:
	HANDLE	m_hKillEvent;
	SOCKET	m_socket;

	CPacket* m_packet;
	AcceptorNode* m_acceptor;
};
