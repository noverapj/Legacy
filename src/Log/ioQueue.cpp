#include "stdafx.h"
#include "ioQueue.h"


ioQueue::ioQueue(void) : m_count(0), m_completionPort(NULL)
{
}

ioQueue::~ioQueue(void)
{
	Cleanup();
}

BOOL ioQueue::Startup( int32 timeout )
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

void ioQueue::Cleanup()
{
	if( m_completionPort )
	{
		CloseHandle( m_completionPort );
		m_completionPort = NULL;
	}	
}

BOOL ioQueue::Enqueue( const DWORD completionKey, const DWORD bytes )
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

BOOL ioQueue::Enqueue( const DWORD completionKey, const DWORD bytes, LPOVERLAPPED ov )
{
	if( m_completionPort )
	{
		if(PostQueuedCompletionStatus(	m_completionPort,
			bytes,
			completionKey,
			ov ))
		{
			InterlockedIncrement( &m_count );
			return TRUE;
		}
	}
	return FALSE;
}

LPDWORD	ioQueue::Dequeue()
{
	DWORD bytes = 0;
	int retVal;
	LPOVERLAPPED ov = NULL;
 	return Dequeue( bytes,ov,retVal );
}

LPDWORD	ioQueue::Dequeue( DWORD &bytes, LPOVERLAPPED &ov, int &retVal)
{
	LPDWORD completionKey = NULL;
	 
	if( m_completionPort )
	{
		retVal = GetQueuedCompletionStatus(	m_completionPort,
			&bytes,
			reinterpret_cast<LPDWORD>(&completionKey),
			&ov,
			m_timeout );
		if(retVal)
		{
			InterlockedDecrement( &m_count );
		}
	}
 	return completionKey;
}
