#pragma once

#include "WorkerThread.h"
#include "LogicThread.h"
#include "UDPWorkerThread.h"
#include <vector>

class CCompletionHandler;

class IOCP_SOCKET_API UDPCompletionHandler : public CCompletionHandler
{
public:
	UDPCompletionHandler(void);
	virtual ~UDPCompletionHandler(void);

	virtual void CreateWorkers(const uint32 count); // for udpworker
};

