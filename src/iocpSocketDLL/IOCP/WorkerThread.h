// WorkerThread.h: interface for the WorkerThread class.

#pragma once

#include "../ThreadModules/Thread.h"
#include "../SocketModules/Packet.h"

class IOCP_SOCKET_API WorkerThread : public Thread  
{
public:
	WorkerThread(HANDLE IOCP);
	virtual ~WorkerThread();

public:
	virtual void Run(); //for udp

protected:
	HANDLE m_IOCP;
	CPacket m_packet;
};
