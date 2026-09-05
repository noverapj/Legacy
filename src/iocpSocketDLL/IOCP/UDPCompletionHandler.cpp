#include "../iocpSocketDLL.h"
#include "UDPCompletionHandler.h"
#include "UDPWorkerThread.h"


UDPCompletionHandler::UDPCompletionHandler(void)
{
}


UDPCompletionHandler::~UDPCompletionHandler(void)
{
}

void UDPCompletionHandler::CreateWorkers( const uint32 count )
{
	m_workerThreads.reserve(count);
	for(uint32 i = 0;i < count;i++)
	{
		m_workerThreads.push_back( new UDPWorkerThread(GetHandle()) );
	}

	for(WORKERS::iterator it = m_workerThreads.begin() ; it != m_workerThreads.end() ; ++it)
	{
		UDPWorkerThread* workerThread = (UDPWorkerThread*)*it;
		if(workerThread)
			workerThread->Begin();
	}
}
