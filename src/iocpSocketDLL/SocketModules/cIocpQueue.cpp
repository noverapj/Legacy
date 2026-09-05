// cIocpQueue.cpp: implementation of the cIocpQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "../iocpSocketDLL.h"
#include "cIocpQueue.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cIocpQueue::cIocpQueue() : m_count(0), m_completionPort(NULL), m_processCount(0)
{
}

cIocpQueue::~cIocpQueue()
{
	Cleanup();
}

//////////////////////////////////////////////////////////////////////
// Startup/Cleanup
//////////////////////////////////////////////////////////////////////
BOOL cIocpQueue::Startup( int32 timeout )
{
	if( !m_completionPort )
	{
		m_completionPort = CreateIoCompletionPort(	INVALID_HANDLE_VALUE, 
													NULL, 
													0, 
													0 );
		if( !m_completionPort ) return FALSE;

		m_timeout = timeout;
		return TRUE;
	}
	
	return FALSE;
}

void	cIocpQueue::Cleanup()
{
	if( m_completionPort )
	{
		CloseHandle( m_completionPort );
		m_completionPort = NULL;
	}	
}


//////////////////////////////////////////////////////////////////////
// Enqueue/Dequeue
//////////////////////////////////////////////////////////////////////
BOOL cIocpQueue::Enqueue( const DWORD completionKey, const DWORD bytes )
{
	if( m_completionPort )
	{
		if(PostQueuedCompletionStatus(	m_completionPort,
										bytes,
										completionKey,
										NULL ))
		{
			InterlockedIncrement( &m_count );
			return TRUE;
		}
	}
	return FALSE;
}

LPDWORD	cIocpQueue::Dequeue()
{
	DWORD bytes = 0;
 	return Dequeue( bytes );
}

LPDWORD	cIocpQueue::Dequeue( DWORD &bytes )
{
	LPDWORD completionKey = NULL;
	LPOVERLAPPED overlapped	= NULL;

	if( m_completionPort )
	{
		if(GetQueuedCompletionStatus(	m_completionPort,
										&bytes,
										reinterpret_cast<LPDWORD>(&completionKey),
										&overlapped,
										m_timeout ))
		{
			InterlockedDecrement( &m_count );
		}
	}
	InterlockedIncrement(&m_processCount);
 	return completionKey;
}



