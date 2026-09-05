#pragma once
 
#include "../SocketModules/Packet.h"

class WorkerThread;

class IOCP_SOCKET_API UDPWorkerThread : public WorkerThread
{
public:
	UDPWorkerThread(HANDLE IOCP);
	virtual ~UDPWorkerThread(void);

public:
	void Run();
};

