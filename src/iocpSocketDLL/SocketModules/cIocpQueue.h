#pragma once

class IOCP_SOCKET_API cIocpQueue  
{
public:
	cIocpQueue();
	virtual ~cIocpQueue();

	BOOL	Startup( int32 timeout = INFINITE );
	void	Cleanup();

public:
	LPDWORD	Dequeue();
	LPDWORD	Dequeue( DWORD &bytes );

	LONG GetCount()	{ return m_count; }

public:
	BOOL Enqueue( const DWORD completionKey, const DWORD bytes );
	long GetProcessCount()
	{
		long val = m_processCount;
		InterlockedExchange(&m_processCount,0);
		return val;
	}

public:
	volatile LONG m_count;

protected:
	int32	m_timeout;
	HANDLE	m_completionPort;
	long	m_processCount;
};