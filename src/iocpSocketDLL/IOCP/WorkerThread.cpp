#include "../iocpSocketDLL.h"
#include "WorkerThread.h"


WorkerThread::WorkerThread(HANDLE IOCP) : m_IOCP(IOCP)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::Run()
{
	while(true)
	{
		DWORD byteTransfer = 0, keyValue = 0;
		LPOVERLAPPED overlapped= NULL;

		BOOL retVal = GetQueuedCompletionStatus( m_IOCP, &byteTransfer, &keyValue, &overlapped, INFINITE );
		if(NULL == overlapped) continue;

		// 로그 관련
		CConnectNode *pParentNode = (CConnectNode*)keyValue;
		IOBufferedContext* lpIOBufferedContext = (IOBufferedContext*)overlapped;
		
		if(ASYNCFLAG_RECEIVE == lpIOBufferedContext->m_flags)
		{
			if((FALSE == retVal) || (0 == byteTransfer))
			{
				if(pParentNode)
					pParentNode->SessionClose(retVal);
				continue;
			}

			if(pParentNode)
				pParentNode->Dispatch( byteTransfer, overlapped, m_packet );
		}
		else if(ASYNCFLAG_SEND == lpIOBufferedContext->m_flags)
		{
			if(pParentNode)
				pParentNode->Dispatch( byteTransfer, overlapped, m_packet );
		}
	}

	PrintTimeAndLog(0,"User WorkerThread[%lu] Thread EXIT", GetCurrentThreadId());
}