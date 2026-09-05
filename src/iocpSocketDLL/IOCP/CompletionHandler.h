// CompletionHandler.h: interface for the CCompletionHandler class.
#pragma once

#include "WorkerThread.h"
#include "LogicThread.h"
#include <vector>

class IOCP_SOCKET_API CCompletionHandler  
{
protected:
	typedef std::vector<WorkerThread*> WORKERS;

	WORKERS m_workerThreads;
	HANDLE m_hIOCP;

public:
	bool CreateIOCP();
	virtual void CreateWorkers(const uint32 count); // for udpworker

public:
	HANDLE GetHandle() { return m_hIOCP; }

	void AddHandleToIOCP(HANDLE handle,DWORD dwKey);
	BOOL GetQueuedStatus( HANDLE CompletionPort, 
						  LPDWORD lpNumberOfBytesTransferred, 
						  LPDWORD lpCompletionKey, 
						  LPOVERLAPPED *lpOverlapped, 
						  DWORD dwMilliseconds);
protected:
	CCompletionHandler();
	virtual ~CCompletionHandler();
};
