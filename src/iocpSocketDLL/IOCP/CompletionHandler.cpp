#include "../iocpSocketDLL.h"
#include "CompletionHandler.h"


CCompletionHandler::CCompletionHandler() : m_hIOCP(0)
{
}

CCompletionHandler::~CCompletionHandler()
{
	for(WORKERS::iterator it = m_workerThreads.begin() ; it != m_workerThreads.end() ; ++it)
	{
		WorkerThread* workerThread = *it;
		if(workerThread)
		{
			delete workerThread;
		}
	}

	m_workerThreads.clear();
}

bool CCompletionHandler::CreateIOCP()
{
	m_hIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if(m_hIOCP == 0)
	{
		PrintTimeAndLog(0,"IOCP CREATE FAILED:%d",GetLastError());
		return false;
	}

	return true;
}

void CCompletionHandler::CreateWorkers(const uint32 count)
{
	m_workerThreads.reserve(count);
	for(uint32 i = 0;i < count;i++)
	{
		m_workerThreads.push_back( new WorkerThread(GetHandle()) );
	}

	for(WORKERS::iterator it = m_workerThreads.begin() ; it != m_workerThreads.end() ; ++it)
	{
		WorkerThread* workerThread = *it;
		if(workerThread)
			workerThread->Begin();
	}
}

void CCompletionHandler::AddHandleToIOCP(HANDLE handle,DWORD dwKey)
{
	::CreateIoCompletionPort(handle,m_hIOCP,dwKey,0);
}

BOOL CCompletionHandler::GetQueuedStatus(HANDLE CompletionPort, 
										 LPDWORD lpNumberOfBytesTransferred, 
										 LPDWORD lpCompletionKey, 
										 LPOVERLAPPED *lpOverlapped, 
										 DWORD dwMilliseconds)
{
	return GetQueuedCompletionStatus( 
		GetHandle(),
		lpNumberOfBytesTransferred,
									  
		lpCompletionKey,
		lpOverlapped,
		dwMilliseconds );
}