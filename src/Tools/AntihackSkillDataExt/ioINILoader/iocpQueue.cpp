#include "StdAfx.h"
#include "iocpQueue.h"


iocpQueue::iocpQueue(void) : m_count(0), m_completionPort(NULL), m_processCount(0), m_timeout(INFINITE)
{
}


iocpQueue::~iocpQueue(void)
{
	Cleanup();
}

bool iocpQueue::Startup()
{
	if( !m_completionPort )
	{
		m_completionPort = CreateIoCompletionPort(	INVALID_HANDLE_VALUE, 
													NULL, 
													0, 
													0 );
		if( !m_completionPort ) return false;

		return true;
	}
	
	return false;
}

void iocpQueue::Cleanup()
{
	if( m_completionPort )
	{
		CloseHandle( m_completionPort );
		m_completionPort = NULL;
	}	
}

BOOL iocpQueue::Enqueue( const DWORD completionKey, const DWORD bytes )
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

LPDWORD	iocpQueue::Dequeue()
{
	DWORD bytes = 0;
 	return Dequeue( bytes );
}

LPDWORD	iocpQueue::Dequeue( DWORD &bytes )
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

