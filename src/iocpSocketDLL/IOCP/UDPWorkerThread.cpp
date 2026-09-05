#include "../iocpSocketDLL.h"
#include "UDPWorkerThread.h"
#include "WorkerThread.h"


UDPWorkerThread::UDPWorkerThread(HANDLE IOCP) : WorkerThread(IOCP)
{
}


UDPWorkerThread::~UDPWorkerThread(void)
{
}

void UDPWorkerThread::Run()
{
	while(1)
	{
		DWORD byteTransfer = 0, keyValue = 0;
		LPOVERLAPPED overlapped= NULL;

		BOOL retVal = GetQueuedCompletionStatus( m_IOCP, &byteTransfer, &keyValue, &overlapped, INFINITE );
		if(NULL == overlapped) continue;

		UDPNode *pParentNode = reinterpret_cast<UDPNode*>(keyValue);
		IOBufferedContext* lpIOBufferedContext = reinterpret_cast<IOBufferedContext*>(overlapped);

		if(ASYNCFLAG_RECEIVE == lpIOBufferedContext->m_flags)
		{
			if(0 == byteTransfer) 
			{
				if(pParentNode)
				{
					int index = pParentNode->FindIndextoOv(overlapped);
					if(index != -1 )
					{
						pParentNode->WaitForPacketReceive(index);
					}
				}
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
	PrintTimeAndLog(0,"User UDPWorkerThread[%lu] Thread EXIT", GetCurrentThreadId());
}